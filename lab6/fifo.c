#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define FIFO_PATH "/tmp/my_fifo"
#define BUFFER_SIZE 256

void get_current_time(char *time_str) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    pid_t pid;
    char buffer[BUFFER_SIZE];
    char time_str[BUFFER_SIZE];
    int fifo_fd;

    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        get_current_time(time_str);
        printf("[Child PID: %d] Current time: %s\n", getpid(), time_str);

        fifo_fd = open(FIFO_PATH, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Failed to open FIFO for reading");
            exit(EXIT_FAILURE);
        }

        if (read(fifo_fd, buffer, BUFFER_SIZE) == -1) {
            perror("Failed to read from FIFO");
            exit(EXIT_FAILURE);
        }

        printf("[Child PID: %d] Received message: %s\n", getpid(), buffer);

        close(fifo_fd);
    } else {
        sleep(5);

        fifo_fd = open(FIFO_PATH, O_WRONLY);
        if (fifo_fd == -1) {
            perror("Failed to open FIFO for writing");
            exit(EXIT_FAILURE);
        }

        get_current_time(time_str);
        snprintf(buffer, BUFFER_SIZE, "[Parent PID: %d] Time: %s", getpid(), time_str);

        if (write(fifo_fd, buffer, strlen(buffer) + 1) == -1) {
            perror("Failed to write to FIFO");
            exit(EXIT_FAILURE);
        }

        close(fifo_fd);

        wait(NULL);

        unlink(FIFO_PATH);
    }

    return 0;
}
