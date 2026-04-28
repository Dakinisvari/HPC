#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
int main()
{
    int *A, *B, *C;
    int i, SIZE;
    printf("Enter the size of arrays: ");
    scanf("%d", &SIZE);
    A = (int*) malloc(SIZE * sizeof(int));
    B = (int*) malloc(SIZE * sizeof(int));
    C = (int*) malloc(SIZE * sizeof(int));
    if (A == NULL || B == NULL || C == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    srand(time(NULL));
    for(i = 0; i < SIZE; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }
    double start1 = omp_get_wtime();
    #pragma omp parallel for
    for(i = 0; i < SIZE; i++) {
        C[i] = A[i] + B[i];
    }
    double end1 = omp_get_wtime();
    printf("\nSample Output:\n");
    for(i = 0; i < 5 && i < SIZE; i++) {
        printf("%d + %d = %d\n", A[i], B[i], C[i]);
    }
    printf("\nTime taken: %f seconds\n", end1 - start1);
    free(A);
    free(B);
    free(C);
    return 0;
}
