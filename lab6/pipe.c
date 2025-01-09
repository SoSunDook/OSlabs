#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 256

void get_current_time(char *time_str) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    char time_str[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        get_current_time(time_str);

        if (read(pipefd[0], buffer, BUFFER_SIZE) == -1) {
            perror("Failed to read from pipe");
            exit(EXIT_FAILURE);
        }

        printf("[Child PID: %d] Current time: %s\n", getpid(), time_str);
        printf("[Child PID: %d] Received message: %s\n", getpid(), buffer);

        close(pipefd[0]);
    } else {
        close(pipefd[0]);

        sleep(5);

        get_current_time(time_str);
        snprintf(buffer, BUFFER_SIZE, "[Parent PID: %d] Time: %s", getpid(), time_str);

        if (write(pipefd[1], buffer, strlen(buffer) + 1) == -1) {
            perror("Failed to write to pipe");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);

        wait(NULL);
    }

    return 0;
}
