#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define MAX 50

typedef struct {
    char name[MAX];
    int rollNo;
    int mark;
    char grade;
} student;

char get_grade(int marks) {
    if (marks >= 90) return 'A';
    if (marks >= 80) return 'B';
    if (marks >= 70) return 'C';
    if (marks >= 60) return 'D';
    if (marks >= 50) return 'E';
    return 'F';
}

void build_mpi_type(student *s, MPI_Datatype *mpi_t) {
    int blocklengths[4] = {MAX, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_CHAR};
    MPI_Aint displacements[4];

    MPI_Get_address(&s[0].name, &displacements[0]);
    MPI_Get_address(&s[0].rollNo, &displacements[1]);
    MPI_Get_address(&s[0].mark, &displacements[2]);
    MPI_Get_address(&s[0].grade, &displacements[3]);

    // Make addresses relative to the start of the struct
    MPI_Aint base;
    MPI_Get_address(s, &base);
    for (int i = 0; i < 4; i++) displacements[i] -= base;

    MPI_Type_create_struct(4, blocklengths, displacements, types, mpi_t);
    MPI_Type_commit(mpi_t);
}

int main(int argc, char** argv) {
    int myrank, commsz, tot = 0;
    student s[MAX];
    MPI_Datatype mpi_t;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsz);

    build_mpi_type(s, &mpi_t);

    if (myrank == 0) {
        FILE *fp = fopen("student.txt", "r");
        if (fp) {
            while (fscanf(fp, "%s %d %d", s[tot].name, &s[tot].rollNo, &s[tot].mark) != EOF) {
                tot++;
            }
            fclose(fp);
        }
    }

    // Broadcast the total count first
    MPI_Bcast(&tot, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = tot / commsz;
    student local[MAX]; // local buffer

    MPI_Scatter(s, local_n, mpi_t, local, local_n, mpi_t, 0, MPI_COMM_WORLD);
    printf("Process %d :\n",myrank);
    for (int i = 0; i < local_n; i++) {
        local[i].grade = get_grade(local[i].mark);
        printf("%s -> Grade: %c\n",local[i].name,local[i].grade);
    }

    MPI_Gather(local, local_n, mpi_t, s, local_n, mpi_t, 0, MPI_COMM_WORLD);

    if (myrank == 0) {
        printf("Name\tRoll no.\tMarks\tGrade\n");
        FILE *f = fopen("grades.txt", "w");
        for (int i = 0; i < tot; i++) {
            fprintf(f, "%s %d %d %c\n", s[i].name, s[i].rollNo, s[i].mark, s[i].grade);
            printf("%s\t%d %d\t%c\n", s[i].name, s[i].rollNo, s[i].mark, s[i].grade);
        }
        fclose(f);
    }

    MPI_Type_free(&mpi_t);
    MPI_Finalize();
    return 0;
}
