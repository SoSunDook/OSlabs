#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <getopt.h>

#define FILENAME_LEN 256
#define HEADER_SIZE (FILENAME_LEN + sizeof(off_t) + sizeof(struct stat))

struct file_header {
    char filename[FILENAME_LEN];
    off_t file_size;
    struct stat file_stat;
};

int add_file_to_archive(const char *archive_name, const char *file_name) {
    int archive_fd, file_fd;
    struct file_header header;
    ssize_t bytes_read, bytes_written;
    char buffer[1024];

    archive_fd = open(archive_name, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (archive_fd < 0) {
        perror("Failed to open archive");
        return -1;
    }

    file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0) {
        perror("Failed to open file");
        close(archive_fd);
        return -1;
    }

    if (fstat(file_fd, &header.file_stat) < 0) {
        perror("Failed to stat file");
        close(file_fd);
        close(archive_fd);
        return -1;
    }

    strncpy(header.filename, file_name, FILENAME_LEN);
    header.file_size = header.file_stat.st_size;

    if (write(archive_fd, &header, sizeof(header)) != sizeof(header)) {
        perror("Failed to write header");
        close(file_fd);
        close(archive_fd);
        return -1;
    }

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(archive_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Failed to write file data");
            close(file_fd);
            close(archive_fd);
            return -1;
        }
    }

    if (bytes_read < 0) {
        perror("Failed to read file");
    }

    close(file_fd);
    close(archive_fd);
    return 0;
}


int extract_file_from_archive(const char *archive_name, const char *file_name) {
    int archive_fd, file_fd;
    struct file_header header;
    ssize_t bytes_read, bytes_written;
    char buffer[1024];

    archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd < 0) {
        perror("Failed to open archive");
        return -1;
    }

    while (read(archive_fd, &header, sizeof(header)) == sizeof(header)) {
        if (strncmp(header.filename, file_name, FILENAME_LEN) == 0) {
            file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, header.file_stat.st_mode);
            if (file_fd < 0) {
                perror("Failed to create extracted file");
                close(archive_fd);
                return -1;
            }

            off_t remaining_size = header.file_size;
            while (remaining_size > 0) {
                bytes_read = read(archive_fd, buffer, sizeof(buffer) < remaining_size ? sizeof(buffer) : remaining_size);
                if (bytes_read <= 0) {
                    perror("Failed to read from archive");
                    close(file_fd);
                    close(archive_fd);
                    return -1;
                }
                bytes_written = write(file_fd, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                    perror("Failed to write to extracted file");
                    close(file_fd);
                    close(archive_fd);
                    return -1;
                }
                remaining_size -= bytes_written;
            }

            close(file_fd);
            close(archive_fd);
            return 0;
        } else {
            lseek(archive_fd, header.file_size, SEEK_CUR);
        }
    }

    close(archive_fd);
    return -1;
}


int show_archive_stat(const char *archive_name) {
    int archive_fd;
    struct file_header header;

    archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd < 0) {
        perror("Failed to open archive");
        return -1;
    }

    while (read(archive_fd, &header, sizeof(header)) == sizeof(header)) {
        printf("File: %s, Size: %ld bytes\n", header.filename, header.file_size);
        lseek(archive_fd, header.file_size, SEEK_CUR);
    }

    close(archive_fd);
    return 0;
}


void print_help() {
    printf("Usage: ./archiver [options] archive_name\n");
    printf("Options:\n");
    printf("  -i, --input FILE      Add FILE to archive\n");
    printf("  -e, --extract FILE    Extract FILE from archive\n");
    printf("  -s, --stat            Show archive status\n");
    printf("  -h, --help            Show this help message\n");
}


int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
            {"input", required_argument, 0, 'i'},
            {"extract", required_argument, 0, 'e'},
            {"stat", no_argument, 0, 's'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
    };

    const char *archive_name = NULL;
    const char *file_name = NULL;
    int show_stat_flag = 0;
    int action_flag = 0;

    while ((opt = getopt_long(argc, argv, "i:e:sh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                file_name = optarg;
                action_flag = 'i';
                break;
            case 'e':
                file_name = optarg;
                action_flag = 'e';
                break;
            case 's':
                show_stat_flag = 1;
                break;
            case 'h':
                print_help();
                exit(0);
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        archive_name = argv[optind];
    } else {
        fprintf(stderr, "Archive name is required\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    if (action_flag == 'i') {
        if (add_file_to_archive(archive_name, file_name) == 0) {
            printf("File '%s' added to archive '%s'.\n", file_name, archive_name);
        } else {
            fprintf(stderr, "Failed to add file '%s' to archive '%s'.\n", file_name, archive_name);
            exit(EXIT_FAILURE);
        }
    } else if (action_flag == 'e') {
        if (extract_file_from_archive(archive_name, file_name) == 0) {
            printf("File '%s' extracted from archive '%s'.\n", file_name, archive_name);
        } else {
            fprintf(stderr, "Failed to extract file '%s' from archive '%s'.\n", file_name, archive_name);
            exit(EXIT_FAILURE);
        }
    } else if (show_stat_flag) {
        if (show_archive_stat(archive_name) == 0) {
            printf("Archive '%s' stat displayed.\n", archive_name);
        } else {
            fprintf(stderr, "Failed to show stat of archive '%s'.\n", archive_name);
            exit(EXIT_FAILURE);
        }
    } else {
        print_help();
        exit(EXIT_FAILURE);
    }

    return 0;
}
