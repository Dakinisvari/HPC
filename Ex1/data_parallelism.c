#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#define N 500
void generateMatrix(int r, int **A)
{
    int i,j;
    for ( i = 0; i < r; i++)
        for ( j = 0; j < r; j++)
            A[i][j] = rand() % 10;
}
void printMatrix(int r, int **A)
{
    int i,j;
    for ( i = 0; i < r; i++) {
        for ( j = 0; j < r; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}
void printSharedMatrix(int n, int (*A)[n])
{
    int i,j;
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < n; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}
int main()
{
    srand(time(NULL));
    int n,i,j,k;
    printf("Enter the dimensions: ");
    scanf("%d", &n);
    int **matA = malloc(n * sizeof(int *));
    int **matB = malloc(n * sizeof(int *));
    int **matC_serial = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++)
    {
        matA[i] = malloc(n * sizeof(int));
        matB[i] = malloc(n * sizeof(int));
        matC_serial[i] = malloc(n * sizeof(int));
    }
    generateMatrix(n, matA);
    generateMatrix(n, matB);
    if (n <= 5)
    {
        printf("\nMatrix A:\n");
        printMatrix(n, matA);
        printf("\nMatrix B:\n");
        printMatrix(n, matB);
    }
    struct timespec start, end;
    /* ----- SERIAL EXECUTION ----- */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for ( i = 0; i < n; i++)
    {
        for ( j = 0; j < n; j++)
        {
            matC_serial[i][j] = 0;
            for ( k = 0; k < n; k++)
            {
                matC_serial[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long serial_time_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
    if (n <= 5)
    {
        printf("\nResult Matrix:\n");
        printMatrix(n, matC_serial);
    }
    /*----- PARALLEL EXECUTION ----- */
    int shmid_mat = shmget(IPC_PRIVATE, sizeof(int) * n * n, IPC_CREAT | 0666);
    int (*matC_parallel)[n] = shmat(shmid_mat, NULL, 0);
    int shmid_time = shmget(IPC_PRIVATE, sizeof(long long) * n, IPC_CREAT | 0666);
    long long *proc_time = shmat(shmid_time, NULL, 0);
    for ( i = 0; i < n; i++)
        proc_time[i] = 0;
    for ( i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            struct timespec c_start, c_end;
            clock_gettime(CLOCK_MONOTONIC, &c_start);
            for ( j = 0; j < n; j++)
            {
                matC_parallel[i][j] = 0;
                for ( k = 0; k < n; k++)
                {
                    matC_parallel[i][j] += matA[i][k] * matB[k][j];
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &c_end);
            proc_time[i] = (c_end.tv_sec - c_start.tv_sec) * 1000000000LL + (c_end.tv_nsec - c_start.tv_nsec);
            exit(0);
        }
    }
    for ( i = 0; i < n; i++)
        wait(NULL);
    long long max_parallel_time_ns = proc_time[0];
    for ( i = 1; i < n; i++) {
        if (proc_time[i] > max_parallel_time_ns)
            max_parallel_time_ns = proc_time[i];
    }
    printf("\nExecution Time (Nanoseconds)\n");
    printf("----------------------------\n");
    printf("Serial Time         : %lld ns\n", serial_time_ns);
    printf("\nParallel Time     : %lld ns\n", max_parallel_time_ns);
    shmdt(matC_parallel);
    shmdt(proc_time);
    shmctl(shmid_mat, IPC_RMID, NULL);
    shmctl(shmid_time, IPC_RMID, NULL);
    return 0;
}
