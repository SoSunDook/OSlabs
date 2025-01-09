#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_file(FILE *file, int number_lines, int number_non_empty, int show_ends) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 1;
    int not_empty = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        if (line[0] == '\0') {
            not_empty = 0;
        } else {
            not_empty = 1;
        }

        if (number_non_empty) {
            if (not_empty) {
                printf("%6d\t", line_number++);
            }
        } else if (number_lines) {
            if (read > 0) {
                printf("%6d\t", line_number++);
            }
        }

        if (show_ends) {
            printf("%s$", line);
        } else{
            printf("%s", line);
        }

        printf("\n");
    }

    free(line);
}

int main(int argc, char *argv[]) {
    int opt;
    int number_lines = 0;
    int number_non_empty = 0;
    int show_ends = 0;

    while ((opt = getopt(argc, argv, "nbE")) != -1) {
        switch (opt) {
            case 'n':
                number_lines = 1;
                break;
            case 'b':
                number_non_empty = 1;
                break;
            case 'E':
                show_ends = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n] [-b] [-E] file...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-n] [-b] [-E] file...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            perror(argv[i]);
            exit(EXIT_FAILURE);
        }

        print_file(file, number_lines, number_non_empty, show_ends);
        fclose(file);
    }

    return 0;
}
