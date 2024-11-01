//"Write a program that displays the same output on the screen as the result of the cat > filename.txt (or cat >> filename.txt) command."

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Correct usage: %s > <filename> OR %s >> <filename>\n", argv[0], argv[0]);
        return 1;
    }

    int fd;
    if (!strcmp(argv[1], ">")) {
        fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, PERMISSION);
    } else {
        fd = open(argv[2], O_WRONLY | O_CREAT | O_APPEND, PERMISSION);
    }
    
    if (fd == -1) {
        fprintf(stderr, "Failed to open file");
        exit(1);
    }

    char* buffer = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == NULL) {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; // Break if there's an error or end-of-file
        }

        int length = strlen(buffer);
        
        if (write(fd, buffer, length) == -1) {
            perror("Error writing to file");
            break;
        }
        memset(buffer, 0, length);
    }

    if (munmap(buffer, 4096) == -1) {
        perror("Error unmapping memory");
    }
    close(fd);
    exit(EXIT_SUCCESS);
}