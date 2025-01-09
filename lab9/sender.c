#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234
#define SHM_KEY 5678

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void signal_handler(int sig) {
    printf("Программа уже запущена.\n");
    exit(1);
}

int main() {
    key_t shm_key = SHM_KEY;
    key_t sem_key = SEM_KEY;
    int shmid, semid;
    struct sembuf sb;
    char *shmaddr;
    time_t now;
    pid_t pid = getpid();

    FILE *f = fopen("/tmp/sender.lock", "r");
    if (f != NULL) {
        fclose(f);
        printf("Процесс с такой программой уже запущен.\n");
        exit(1);
    }
    f = fopen("/tmp/sender.lock", "w");
    if (f == NULL) {
        perror("Ошибка при создании файла блокировки");
        exit(1);
    }
    fprintf(f, "%d\n", pid);
    fclose(f);

    shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) == -1) {
        perror("semctl");
        exit(1);
    }

    while (1) {
        now = time(NULL);

        sb.sem_num = 0;
        sb.sem_op = -1;
        sb.sem_flg = 0;
        semop(semid, &sb, 1);

        snprintf(shmaddr, SHM_SIZE, "Time: %sPID: %d", ctime(&now), pid);

        sb.sem_op = 1;
        semop(semid, &sb, 1);

        sleep(1);
    }

    return 0;
}
