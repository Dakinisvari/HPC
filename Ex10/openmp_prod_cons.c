#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define MAX_QUEUE 100
#define MAX_LINE 256
int NUM_PRODUCERS;
int NUM_CONSUMERS;
char queue[MAX_QUEUE][MAX_LINE];
int front = 0, rear = 0, count = 0;
int turn = 0; // 0 = producer, 1 = consumer
int done_producers = 0;
omp_lock_t lock;
void enqueue(char *line)
{
    strcpy(queue[rear], line);
    rear = (rear + 1) % MAX_QUEUE;
    count++;
}
int dequeue(char *line)
{
    if (count > 0)
    {
        strcpy(line, queue[front]);
        front = (front + 1) % MAX_QUEUE;
        count--;
        return 1;
    }
    return 0;
}
void producer(int id, char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        printf("Producer %d: Cannot open %s\n", id, filename);
        return;
    }

    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, fp))
    {
        int done = 0;

        while (!done)
        {
            omp_set_lock(&lock);

            if (turn == 0 && count < MAX_QUEUE)
            {
                enqueue(line);
                printf("Producer %d produced: %s", id, line);

                turn = 1; // give turn to consumer
                done = 1;
            }

            omp_unset_lock(&lock);
        }
    }

    fclose(fp);

    #pragma omp atomic
    done_producers++;   // mark producer finished
}
void consumer(int id)
{
    char line[MAX_LINE];

    while (1)
    {
        int done = 0;

        while (!done)
        {
            omp_set_lock(&lock);

            // EXIT CONDITION
            if (done_producers == NUM_PRODUCERS && count == 0)
            {
                omp_unset_lock(&lock);
                return;
            }

            if (turn == 1 && count > 0)
            {
                if (dequeue(line))
                {
                    char *token = strtok(line, " \t\n");

                    while (token != NULL)
                    {
                        printf("Consumer %d consumed: %s\n", id, token);
                        token = strtok(NULL, " \t\n");
                    }

                    turn = 0; // back to producer
                    done = 1;
                }
            }

            omp_unset_lock(&lock);
        }
    }
}
int main()
{
    printf("Enter number of producers: ");
    scanf("%d", &NUM_PRODUCERS);

    printf("Enter number of consumers: ");
    scanf("%d", &NUM_CONSUMERS);

    char *files[3] = {"file1.txt", "file2.txt", "file3.txt"};

    omp_init_lock(&lock);

    #pragma omp parallel num_threads(NUM_PRODUCERS + NUM_CONSUMERS)
    {
        int tid = omp_get_thread_num();

        if (tid < NUM_PRODUCERS)
        {
            producer(tid, files[tid]);
        }
        else
        {
            consumer(tid - NUM_PRODUCERS);
        }
    }

    omp_destroy_lock(&lock);

    return 0;
}