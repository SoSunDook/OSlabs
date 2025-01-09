#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define SHM_SIZE 1024

int main() {
    key_t key;
    int shmid;
    char *shm, *s;

    const char *filename = "shmfile";
    int fd = open(filename, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    close(fd);

    if ((key = ftok(filename, 65)) == -1) {
        perror("ftok");
        exit(1);
    }

    int lock_fd = open("/tmp/sender.lock", O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("open lock file");
        exit(1);
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(lock_fd, F_SETLK, &fl) == -1) {
        printf("Отправитель уже запущен\n");
        exit(1);
    }

    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) == -1) {
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

        snprintf(shm, SHM_SIZE, "PID: %d, Time: %02d:%02d:%02d", getpid(), t->tm_hour, t->tm_min, t->tm_sec);
        sleep(1);
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
