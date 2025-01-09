#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"


void print_permissions(const struct stat *file_stat) {
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
}


void print_owner_group_size(const struct stat *file_stat) {
    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group *gr = getgrgid(file_stat->st_gid);

    if (pw != NULL && gr != NULL) {
        printf(" %-8s %-8s", pw->pw_name, gr->gr_name);
    } else {
        printf(" %-8d %-8d", file_stat->st_uid, file_stat->st_gid);
    }

    printf(" %8ld", (long)file_stat->st_size);
}


void print_modification_time(const struct stat *file_stat) {
    char timebuf[80];
    struct tm *tm_info = localtime(&file_stat->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
    printf(" %s", timebuf);
}


void print_colored_filename(const struct stat *file_stat, const char *name) {
    if (S_ISDIR(file_stat->st_mode)) {
        printf(COLOR_BLUE "%s" COLOR_RESET, name);
    } else if (S_ISLNK(file_stat->st_mode)) {
        printf(COLOR_CYAN "%s" COLOR_RESET, name);
    } else if (file_stat->st_mode & S_IXUSR) {
        printf(COLOR_GREEN "%s" COLOR_RESET, name);
    } else {
        printf("%s", name);
    }
}


void print_file_info(const char *path, const char *name, bool long_format) {
    struct stat file_stat;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

    if (stat(full_path, &file_stat) == -1) {
        perror("stat");
        return;
    }

    if (long_format) {
        print_permissions(&file_stat);
        printf(" %2ld", (long)file_stat.st_nlink);
        print_owner_group_size(&file_stat);
        print_modification_time(&file_stat);
        printf(" ");
    }

    print_colored_filename(&file_stat, name);
    printf("\n");
}


void list_directory(const char *path, bool all_files, bool long_format) {
    struct dirent **namelist;
    int n;

    n = scandir(path, &namelist, NULL, alphasort);
    if (n == -1) {
        perror("scandir");
        return;
    }

    for (int i = 0; i < n; i++) {
        if (!all_files && namelist[i]->d_name[0] == '.') {
            free(namelist[i]);
            continue;
        }

        print_file_info(path, namelist[i]->d_name, long_format);
        free(namelist[i]);
    }

    free(namelist);
}


int main(int argc, char *argv[]) {
    bool all_files = false;
    bool long_format = false;
    int opt;


    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                long_format = true;
                break;
            case 'a':
                all_files = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-a] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    list_directory(path, all_files, long_format);

    return 0;
}
