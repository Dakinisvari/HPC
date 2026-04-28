#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define BUFFER_SIZE 5
#define MAX_WORDS 10
#define NUM_THREADS 3
char *buffer[BUFFER_SIZE];
char *result_buffer[MAX_WORDS];
char *dictionary[] = {"apple", "banana", "grape", "orange", "mango"};
int dict_size = 5;
int done = 0;
int count = 0;
int in = 0, out = 0;
int result_count = 0;
pthread_mutex_t lock;
pthread_cond_t not_empty;
pthread_cond_t not_full;
int spell_check(char *word)
{
    for (int i = 0; i < dict_size; i++)
    {
        if (strcmp(word, dictionary[i]) == 0)
                return 1;
    }
    return 0;
}
void *spell_checker(void *arg)
{
    int id = *(int *)arg;
    while (1)
    {
        pthread_mutex_lock(&lock);
        while (count == 0 && !done)
            pthread_cond_wait(&not_empty, &lock);
        if (count == 0 && done)
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        char *word = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);
        int result = spell_check(word);
        char *res = malloc(50);
        sprintf(res, "Thread %d processed: %s -> %s",
                id, word, result ? "Correct" : "Incorrect");
        pthread_mutex_lock(&lock);
        result_buffer[result_count++] = res;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
void *producer(void *arg)
{
    char *words[MAX_WORDS] = {"apple", "baananna", "grape", "mango",
        "banana", "oranngge","maango","guava","cherry","grpe"
    };
    for (int i = 0; i < MAX_WORDS; i++)
    {
        pthread_mutex_lock(&lock);
        while (count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &lock);
        buffer[in] = words[i];
        in = (in + 1) % BUFFER_SIZE;
        count++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }
    pthread_mutex_lock(&lock);
    done = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&lock);
    return NULL;
}
int main()
{
    pthread_t prod;
    int ids[NUM_THREADS];
    pthread_t cons[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
    {
            ids[i] = i + 1;
            pthread_create(&cons[i], NULL, spell_checker, &ids[i]);
    }
    pthread_join(prod, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(cons[i], NULL);
    printf("\nSpell Check Results:\n");
    for (int i = 0; i < result_count; i++)
    {
        printf("%s\n", result_buffer[i]);
        free(result_buffer[i]);
    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    return 0;
}
