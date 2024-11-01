//Write a program that creates two child processes that can access the parent's shared memory. 
//The parent process should write a message to shared memory, 
//which the first child process will read and display, and then take any input provided by the user. 
//The parent will wait for the first child to complete normally before creating the second child process. 
//The second child will perform a calculation based on an expression, 
//and if successful, the parent will display the result on the screen.

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/wait.h> 
#include <errno.h>
#include <string.h>

volatile int counter = 0;
void handler(int signum); 
void alarmHandler(int signum);

int main() {
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {//child
        signal(SIGUSR1, handler);
        signal(SIGALRM, alarmHandler);
        alarm(10);
        while (1) {
        }
        exit(EXIT_SUCCESS);
    } 

    char interupt[4];
    printf("Interupt the child process or no? (type YES or NO): ");
    scanf("%s", interupt);
    int count = 2;
    while (!strcmp(interupt, "NO")) {
        int tmp_count = count;
        while (tmp_count--) {
            kill(pid, SIGUSR1);
            sleep(1);
        }
        ++count;
        printf("Interupt the child process or no? (type YES or NO): ");
        scanf("%s", interupt);
    }
    kill (pid, SIGKILL);
    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        printf("The child with pid %d exited with normal exit status.\n", pid);
    } else if (WIFSIGNALED(status)) {
        printf("The child with pid %d exited with signal %s.\n", pid, strsignal(WTERMSIG(status)));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void handler(int signum)//SIGUSR1
{
    ++counter;
}

void alarmHandler(int signum) 
{
    printf("At the current moment child reseived %d SIGUSR1 signals.\n", counter);
    alarm(10);
}