#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>


void print_usage() {
    printf("Usage:\n");
    printf("  ./customchmod +x file.txt\n");
    printf("  ./customchmod u-r file.txt\n");
    printf("  ./customchmod g+rw file.txt\n");
    printf("  ./customchmod 766 file.txt\n");
}

void change_permissions(const char *path, const char *mode) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("Error getting file stats");
        exit(1);
    }

    mode_t new_mode = st.st_mode;

    if (mode[0] == '+') {
        if (strchr(mode, 'x')) new_mode |= S_IXUSR;
        if (strchr(mode, 'r')) new_mode |= S_IRUSR;
        if (strchr(mode, 'w')) new_mode |= S_IWUSR;
        if (strchr(mode, 'X')) new_mode |= S_IXGRP | S_IXOTH;
    }
    else if (mode[0] == '-') {
        if (strchr(mode, 'x')) new_mode &= ~S_IXUSR;
        if (strchr(mode, 'r')) new_mode &= ~S_IRUSR;
        if (strchr(mode, 'w')) new_mode &= ~S_IWUSR;
        if (strchr(mode, 'X')) new_mode &= ~(S_IXGRP | S_IXOTH);
    }
    else if (isdigit(mode[0])) {
        new_mode = strtol(mode, NULL, 8);
    } else {
        char who = mode[0]; // u, g, o, or a
        char op = mode[1];  // + or -
        char perm = mode[2]; // r, w, or x

        if (who == 'u') {
            if (op == '+') {
                if (perm == 'r') new_mode |= S_IRUSR;
                if (perm == 'w') new_mode |= S_IWUSR;
                if (perm == 'x') new_mode |= S_IXUSR;
            } else if (op == '-') {
                if (perm == 'r') new_mode &= ~S_IRUSR;
                if (perm == 'w') new_mode &= ~S_IWUSR;
                if (perm == 'x') new_mode &= ~S_IXUSR;
            }
        } else if (who == 'g') {
            if (op == '+') {
                if (perm == 'r') new_mode |= S_IRGRP;
                if (perm == 'w') new_mode |= S_IWGRP;
                if (perm == 'x') new_mode |= S_IXGRP;
            } else if (op == '-') {
                if (perm == 'r') new_mode &= ~S_IRGRP;
                if (perm == 'w') new_mode &= ~S_IWGRP;
                if (perm == 'x') new_mode &= ~S_IXGRP;
            }
        } else if (who == 'o') {
            if (op == '+') {
                if (perm == 'r') new_mode |= S_IROTH;
                if (perm == 'w') new_mode |= S_IWOTH;
                if (perm == 'x') new_mode |= S_IXOTH;
            } else if (op == '-') {
                if (perm == 'r') new_mode &= ~S_IROTH;
                if (perm == 'w') new_mode &= ~S_IWOTH;
                if (perm == 'x') new_mode &= ~S_IXOTH;
            }
        }
    }

    if (chmod(path, new_mode) != 0) {
        perror("Error changing file permissions");
        exit(1);
    }

    printf("Permissions changed successfully for %s\n", path);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    const char *mode = argv[1];
    const char *file = argv[2];

    change_permissions(file, mode);

    return 0;
}
