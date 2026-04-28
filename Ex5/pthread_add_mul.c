#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 10
int A[MAX][MAX], B[MAX][MAX];
int add[MAX][MAX], mul[MAX][MAX];
int r1, c1, r2, c2;
void *matrix_addition(void *arg)
{
    printf("\nThread 1 performing Matrix Addition\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            add[i][j] = A[i][j] + B[i][j];
        }
    }
    printf("Thread 1 completed addition\n");
    pthread_exit(NULL);
}
void *matrix_multiplication(void *arg)
{
    printf("\nThread 2 performing Matrix Multiplication\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            mul[i][j] = 0;
            for (int k = 0; k < c1; k++) {
                mul[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    printf("Thread 2 completed multiplication\n");
    pthread_exit(NULL);
}
int main()
{
    pthread_t t1, t2;
    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &r2, &c2);
    if (c1 != r2)
    {
        printf("Multiplication not possible\n");
        return 0;
    }
    printf("Enter Matrix A:\n");
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c1; j++)
            scanf("%d", &A[i][j]);
    printf("Enter Matrix B:\n");
    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++)
            scanf("%d", &B[i][j]);
    pthread_create(&t1, NULL, matrix_addition, NULL);
    pthread_create(&t2, NULL, matrix_multiplication, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\nFinal Matrix Addition Result:\n");
    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c1; j++)
        {
            printf("%d ", add[i][j]);
        }
        printf("\n");
    }
    printf("\nFinal Matrix Multiplication Result:\n");
    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c2; j++)
        {
            printf("%d ", mul[i][j]);
        }
        printf("\n");
    }
    return 0;
}
