#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef struct {
    int a, b;
    int op; // 0: +, 1: -, 2: *, 3: /
} WorkItem;

// Shared Buffer and Synchronization
WorkItem work_buffer[BUFFER_SIZE];
int in = 0, out = 0;
sem_t empty, full, mutex;

// Worker Logic
void* compute_worker(void* arg) {
    int my_op = *(int*)arg;
    char ops[] = {'+', '-', '*', '/'};

    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        // Check if the item at 'out' matches this thread's operation
        if (work_buffer[out].op == my_op) {
            WorkItem item = work_buffer[out];
            int result = 0;

            if (my_op == 0) result = item.a + item.b;
            else if (my_op == 1) result = item.a - item.b;
            else if (my_op == 2) result = item.a * item.b;
            else if (my_op == 3) result = (item.b != 0) ? item.a / item.b : 0;

            // Safe file writing: Open and close within the thread
            FILE *f = fopen("result_file.txt", "a");
            if (f) {
                fprintf(f, "Thread : %d %c %d = %d\n",  item.a, ops[my_op], item.b, result);
                fclose(f);
            }
            printf("Thread  processed: %d %c %d = %d\n",  item.a, ops[my_op], item.b, result);

            out = (out + 1) % BUFFER_SIZE;
            sem_post(&mutex);
            sem_post(&empty);
        } else {
            // Item is not for me; put it back for another thread
            sem_post(&mutex);
            sem_post(&full);
            usleep(100); // Avoid CPU pinning
        }
    }
    return NULL;
}

void* server_producer(void* arg) {
    while (1) {
        sem_wait(&empty);
        sem_wait(&mutex);

        work_buffer[in].a = rand() % 100;
        work_buffer[in].b = (rand() % 99) + 1;
        work_buffer[in].op = rand() % 4;

        //printf("Server Generated:\n");
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t server;
    pthread_t workers[4]; // FIX: Properly declared array
    int op_ids[4] = {0, 1, 2, 3}; // FIX: Properly declared array
    int i;
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    // Initialise file
    FILE *f = fopen("result_file.txt", "w");
    if(f) { fprintf(f, "Calculator \n"); fclose(f); }

    pthread_create(&server, NULL, server_producer, NULL);
    for ( i = 0; i < 4; i++) {
        pthread_create(&workers[i], NULL, compute_worker, &op_ids[i]);
    }

    pthread_join(server, NULL);
    return 0;
}
