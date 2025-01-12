#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define BUFFER_SIZE 16

char buffer[BUFFER_SIZE];
int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void *writer_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        snprintf(buffer, BUFFER_SIZE, "Count: %d", counter++);
        printf("[Writer] Updated buffer: %s\n", buffer);

        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void *reader_thread(void *arg) {
    long tid = (long)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        pthread_cond_wait(&cond, &mutex);

        printf("[Reader %ld] Buffer: %s\n", tid, buffer);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[NUM_READERS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&writer, NULL, writer_thread, NULL);

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void *)i);
    }

    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
