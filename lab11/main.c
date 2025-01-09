#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define BUFFER_SIZE 16

char buffer[BUFFER_SIZE];
int counter = 0;
pthread_rwlock_t rwlock;

void *writer_thread(void *arg) {
    while (1) {
        pthread_rwlock_wrlock(&rwlock);

        snprintf(buffer, BUFFER_SIZE, "Count: %d", counter++);
        printf("[Writer] Updated buffer: %s\n", buffer);

        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }
    return NULL;
}

void *reader_thread(void *arg) {
    long tid = (long)arg;

    while (1) {
        pthread_rwlock_rdlock(&rwlock);

        printf("[Reader %ld] Buffer: %s\n", tid, buffer);

        pthread_rwlock_unlock(&rwlock);

        usleep(500000);
    }
    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[NUM_READERS];

    pthread_rwlock_init(&rwlock, NULL);

    pthread_create(&writer, NULL, writer_thread, NULL);

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void *)i);
    }

    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
