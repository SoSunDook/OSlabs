#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 5
#define NUM_READERS 10

char shared_array[ARRAY_SIZE];
pthread_mutex_t mutex;
pthread_cond_t cond_var;
int updated = 0;


void* reader(void* arg) {
    long tid = (long)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (!updated) {
            pthread_cond_wait(&cond_var, &mutex);
        }

        printf("Reader %ld: ", tid);
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            printf("%c", shared_array[i]);
        }
        printf("\n");

        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
    return NULL;
}


void* writer(void* arg) {
    char current_digit = '0';
    while (1) {
        pthread_mutex_lock(&mutex);

        for (int i = 0; i < ARRAY_SIZE; ++i) {
            shared_array[i] = current_digit;
        }

        updated = 1;
        pthread_cond_broadcast(&cond_var);

        current_digit++;
        if (current_digit > '9') {
            current_digit = '0';
        }

        pthread_mutex_unlock(&mutex);
        usleep(1000000);
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writer_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    pthread_create(&writer_thread, NULL, writer, NULL);

    for (long i = 0; i < NUM_READERS; ++i) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }

    pthread_join(writer_thread, NULL);
    for (int i = 0; i < NUM_READERS; ++i) {
        pthread_join(readers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}
