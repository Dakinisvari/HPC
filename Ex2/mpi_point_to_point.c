#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
const int MAX = 100;
int isPalindrome(char str[]) {
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j)
    {
        if (str[i] != str[j])
            return 0;
        i++;
        j--;
    }
    return 1;
}
int main()
{
    char msg[MAX];
    int comm_sz;
    int my_rank;
    MPI_Status status;
    char *string_list[] = {"hello","level","madam","computer","people"};
    int list_size = 5;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank != 0)
    {
        strcpy(msg, string_list[my_rank % list_size]);
        if (my_rank % 2 == 1)
        {
            MPI_Send(msg, strlen(msg) + 1,MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(msg, strlen(msg) + 1,MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        }
    }
    else
    {
        char recv_msg[MAX];
        int q;
        for ( q = 1; q < comm_sz; q++)
        {
            MPI_Recv(recv_msg, MAX, MPI_CHAR,MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 1)
            {
                struct timeval s,e;
                double time_used;
                gettimeofday(&s,NULL);
                int i;
                for ( i = 0; recv_msg[i]; i++)
                    recv_msg[i] = toupper(recv_msg[i]);
                printf("Process %d\tIn uppercase: %s\n",status.MPI_SOURCE, recv_msg);
                gettimeofday(&e,NULL);
                time_used=(e.tv_sec-s.tv_sec)*1e6;
                time_used=(time_used+(e.tv_sec-s.tv_sec))*1e-6;
                time_used*=1000000000;
                printf("Execution time for converting into uppercase: %f\n",time_used);

            }
            else if (status.MPI_TAG == 2)
            {
                struct timeval s,e;
                double time_used;
                gettimeofday(&s,NULL);
                if (isPalindrome(recv_msg))
                    printf("Process %d\tIs a palindrome: %s\n",status.MPI_SOURCE, recv_msg);
                else
                    printf("Process %d\tNot a palindrome: %s\n",status.MPI_SOURCE, recv_msg);
                gettimeofday(&e,NULL);
                time_used=(e.tv_sec-s.tv_sec)*1e6;
                time_used=(time_used+(e.tv_sec-s.tv_sec))*1e-6;
                time_used*=1000000000;
                printf("Execution time for checking palindrome: %f\n",time_used);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
