//Write a program that creates two child processes that can access the parent's shared memory. 
//The parent process should write a message to shared memory, 
//which the first child process will read and display, and then take any input provided by the user. 
//The parent will wait for the first child to complete normally before creating the second child process. 
//The second child will perform a calculation based on an expression, 
//and if successful, the parent will display the result on the screen.

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void calculate(char* sh_mem) {
    char* token = strtok(sh_mem, " ");
    int num1 = atoi(token);

    char* operator = strtok(NULL, " ");

    token = strtok(NULL, " ");
    int num2 = atoi(token);

    int res = 0;
    switch (operator[0])
    {
    case '+':
        res = num1 + num2;
        break;
    case '-':
        res = num1 - num2;
        break;
    case '*':
        res = num1 * num2;
        break;
    case '/':
        res = num1 / num2;
        break;
    default:
        break;
    }

    sprintf(sh_mem, "%d", res);
}

int main()
{
    char *sh_mem = (char*)mmap(NULL, 4096, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (sh_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    printf("Enter the expression with this format -> x ? y, where ? can be +, -, *, /: ");
    fgets(sh_mem, 4096, stdin);

    int status;
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        munmap(sh_mem, 4096);
        return 1;
    } else if (pid == 0) {
        calculate(sh_mem);
        exit(0);
    } else {
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Expression calculation result: %s.\n", sh_mem);
            munmap(sh_mem, 4096);
            exit(EXIT_SUCCESS);
        } else if (WIFSIGNALED(status)) {
            printf("Process with pid %d exited with signal %d: %s\n",
                    pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
            munmap(sh_mem, 4096);
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_FAILURE);
}