#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>


int main() {

    int fd[2];
    pipe(fd);
    pid_t p1,p2;
    p1 = fork();

    if (p1 < 0)
    {
    fprintf(stderr, "fork Failed" );
    return 1;
    }

    //child
    if (p1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }

    else{
    while(wait(NULL) > 0);
    }

    p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        close(fd[2]);
        close(fd[3]);
        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }
    return 0;
}