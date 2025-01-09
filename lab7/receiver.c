#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_SIZE 1024

int main() {
    key_t key;
    int shmid;
    char *shm;

    if ((key = ftok("shmfile", 65)) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, SHM_SIZE, 0666)) == -1) {
        perror("shmget");
        exit(1);
    }

    shm = shmat(shmid, (void *)0, 0);
    if (shm == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        printf("Receiver PID: %d, Current Time: %02d:%02d:%02d, Received: %s\n", getpid(), t->tm_hour, t->tm_min, t->tm_sec, shm);
        sleep(1);
    }

    shmdt(shm);

    return 0;
}
