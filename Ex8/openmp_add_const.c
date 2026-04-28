#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
int main() {
    int *A;
    int SIZE, i;
    int constant;
    printf("Enter the size of array: ");
    scanf("%d", &SIZE);

    printf("Enter the constant value to add: ");
    scanf("%d", &constant);

    // Allocate memory
    A = (int*) malloc(SIZE * sizeof(int));
    if (A == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Seed random generator
    srand(time(NULL));

    // Initialize array with random values
    for(i = 0; i < SIZE; i++) {
        A[i] = rand() % 100;
    }

    double start = omp_get_wtime();

    // Parallel loop to add constant
    #pragma omp parallel for
    for(i = 0; i < SIZE; i++) {
        A[i] = A[i] + constant;
    }

    double end = omp_get_wtime();

    // Print sample output
    printf("\nSample Output (first 5 elements):\n");
    for(i = 0; i < 5 && i < SIZE; i++) {
        printf("%d ", A[i]);
    }

    printf("\n\nTime taken: %f seconds\n", end - start);

    // Free memory
    free(A);

    return 0;
}
