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

        free(namelist[i]);
    }

    free(namelist);
}


int main(int argc, char *argv[]) {
    bool all_files = false;
    bool long_format = false;

    const char *path = (optind < argc) ? argv[optind] : ".";

    list_directory(path, all_files, long_format);

    return 0;
}
