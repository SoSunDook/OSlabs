#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234
#define SHM_KEY 5678

struct sembuf sb;

int main() {
    key_t shm_key = SHM_KEY;
    key_t sem_key = SEM_KEY;
    int shmid, semid;
    char *shmaddr;
    time_t now;
    pid_t pid = getpid();

    shmid = shmget(shm_key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    semid = semget(sem_key, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    while (1) {
        now = time(NULL);

        sb.sem_num = 0;
        sb.sem_op = -1;
        sb.sem_flg = 0;
        semop(semid, &sb, 1);

        printf("Receiver PID: %d\nCurrent Time: %sReceived: %s\n", pid, ctime(&now), shmaddr);

        sb.sem_op = 1;
        semop(semid, &sb, 1);

        sleep(1);
    }

    return 0;
}
