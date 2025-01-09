#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int matches_pattern(const char *line, const char *pattern) {
    return strstr(line, pattern) != NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s pattern [file]\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    FILE *file;

    if (argc == 3) {
        file = fopen(argv[2], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
    } else {
        file = stdin;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        if (matches_pattern(line, pattern)) {
            printf("%s", line);
        }
    }

    if (file != stdin) {
        fclose(file);
    }

    return 0;
}
