#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handle_sigint(int sig) {
    printf("Process [pid: %d] caught SIGINT (signal number: %d)\n", getpid(), sig);
    exit(EXIT_SUCCESS);
}

void handle_sigterm(int sig, siginfo_t *info, void *context) {
    printf("Process [pid: %d] caught SIGTERM (signal number: %d)\n", getpid(), sig);
    printf("Additional info: %d\n", info->si_value.sival_int);
    exit(EXIT_SUCCESS);
}

void process_exit(void) {
    printf("Process [pid: %d] is exiting...\n", getpid());
}

int main() {
    if (atexit(process_exit) != 0) {
        perror("atexit registration failed");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Failed to register SIGINT handler");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_sigaction = handle_sigterm;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Failed to register SIGTERM handler");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child process [pid: %d] running...\n", getpid());
        sleep(3);
        exit(42);
    } else {
        printf("Parent process [pid: %d] created child with PID %d\n", getpid(), pid);

        int status;
        pid_t child_pid = wait(&status);

        if (child_pid > 0) {
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                printf("Child process [pid: %d] exited with status %d\n", child_pid, exit_status);
            } else {
                printf("Child process [pid: %d] did not exit normally\n", child_pid);
            }
        } else {
            perror("wait() failed");
        }

        sleep(7);
    }

    return 0;
}
