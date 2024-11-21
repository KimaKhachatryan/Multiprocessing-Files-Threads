//Write a program that:

//Opens a file and maps it into memory using mmap() with the MAP_SHARED flag.
//Creates child processes such that each process works with a separate 1024-byte segment of the file.
//Asks the user which word in the file they want to replace and with what word.
//Each child process should perform the replacement in its assigned segment of the file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


void replace_word(char *segment, const char *old_word, const char *new_word, size_t length) {
    char *pos = segment;
    while ((pos = strstr(pos, old_word)) != NULL) {
        size_t old_len = strlen(old_word);
        size_t new_len = strlen(new_word);

        if (new_len <= old_len) {
            memcpy(pos, new_word, new_len);
            if (new_len < old_len) {
                memset(pos + new_len, ' ', old_len - new_len);
            }
        }
        pos += old_len;
    }
}

void read_word(char *old_word, char *new_word, int size) 
{
    printf("Enter the word to replace: ");
    scanf("%s", old_word);

    printf("Enter the new word: ");
    scanf("%s", new_word);
}

void parent_handler(int signum)
{
    int status;
    wait(&status);
    if (WIFSIGNALED(status)) {
        fprintf(stderr, "Something is wrong...\nThe program may give an incorrect result.\n");
    }
}

int flag = 1;
void child_handler(int signum)
{
    flag = 0;
}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Ccorrect usage: ./ChangeWord <FileName>\n");
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[1];
    
    int fd = open(fileName, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    size_t file_size = st.st_size;
    char *mapped = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char old_word[50];
    char new_word[50];

    pid_t pid = fork();
    if (pid < 0) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    } else if (pid > 0) { //parent
        signal(SIGUSR1, parent_handler);
        sleep(2);
        read_word(old_word, new_word, 50);
        kill(pid, SIGUSR2);
        replace_word(mapped, old_word, new_word, file_size);
        munmap(mapped, file_size);
        close(fd);
        pause();
    } else {//child
        signal(SIGUSR2, child_handler);
        while (flag) {}
        read_word(old_word, new_word, 50);
        replace_word(mapped, old_word, new_word, file_size);
        munmap(mapped, file_size);
        close(fd);
        kill(getppid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}