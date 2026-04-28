#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
void generateMatrix(int n, int **A)
{
    int i,j;
    for ( i = 0; i < n; i++)
        for ( j = 0; j < n; j++)
            A[i][j] = rand() % 10;
}
void printMatrix(int n, int **A)
{
    int i,j;
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}
long long diff_ns(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec) * 1000000000LL + (e.tv_nsec - s.tv_nsec);
}
int main()
{
    srand(time(NULL));
    int n,i,j,k;
    printf("Enter matrix dimensions: ");
    scanf("%d", &n);
    int **matA = malloc(n * sizeof(int *));
    int **matB = malloc(n * sizeof(int *));
    int **mat_add = malloc(n * sizeof(int *));
    int **mat_sub = malloc(n * sizeof(int *));
    for ( i = 0; i < n; i++)
    {
        matA[i] = malloc(n * sizeof(int));
        matB[i] = malloc(n * sizeof(int));
        mat_add[i] = malloc(n * sizeof(int));
        mat_sub[i] = malloc(n * sizeof(int));
    }
    generateMatrix(n, matA);
    generateMatrix(n, matB);
    if (n <= 5)
    {
        printf("\nMatrix A:\n"); printMatrix(n, matA);
        printf("\nMatrix B:\n"); printMatrix(n, matB);
    }
    struct timespec start, end;
    /* ----- SERIAL ----- */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for ( i = 0; i < n; i++)
        for ( j = 0; j < n; j++)
            mat_add[i][j] = matA[i][j] + matB[i][j];
    for ( i = 0; i < n; i++)
        for ( j = 0; j < n; j++)
            mat_sub[i][j] = matA[i][j] - matB[i][j];
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long serial_time_ns = diff_ns(start, end);
    if(n<=5)
    {
       printf("\nAddition result:\n");
       printMatrix(n, mat_add);
       printf("\nSubtraction result:\n");
       printMatrix(n, mat_sub);
    }
    /* ----- PARALLEL ----- */
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0)
    {
        /* CHILD – subtraction */
        close(fd[0]);
        struct timespec cs, ce;
        clock_gettime(CLOCK_MONOTONIC, &cs);
        for ( i = 0; i < n; i++)
            for ( j = 0; j < n; j++)
                mat_sub[i][j] = matA[i][j] - matB[i][j];
        clock_gettime(CLOCK_MONOTONIC, &ce);
        long long child_time = diff_ns(cs, ce);
        write(fd[1], &child_time, sizeof(child_time));
        close(fd[1]);
        exit(0);
    }
    /* PARENT – addition */
    close(fd[1]);
    struct timespec ps, pe;
    clock_gettime(CLOCK_MONOTONIC, &ps);
    for ( i = 0; i < n; i++)
        for ( j = 0; j < n; j++)
            mat_add[i][j] = matA[i][j] + matB[i][j];
    clock_gettime(CLOCK_MONOTONIC, &pe);
    long long parent_time = diff_ns(ps, pe);
    long long child_time;
    read(fd[0], &child_time, sizeof(child_time));
    close(fd[0]);
    wait(NULL);
    long long parallel_time_ns = (parent_time > child_time) ? parent_time : child_time;
    printf("\nExecution Time (Nanoseconds)\n");
    printf("----------------------------\n");
    printf("Serial Time        : %lld ns\n", serial_time_ns);
    printf("Parallel Time(MAX) : %lld ns\n", parallel_time_ns);
    for (i = 0; i < n; i++)
    {
        free(matA[i]); free(matB[i]);
        free(mat_add[i]); free(mat_sub[i]);
    }
    free(matA); free(matB); free(mat_add); free(mat_sub);
    return 0;
}
