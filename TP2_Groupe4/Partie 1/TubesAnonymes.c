
/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * TubesAnonymes.c
 * Ghobrial, Mina Youhanna Helmi / 2214988
 * Rouabah, Abdelmounaim / 2211513
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int pipe1[2];
    pipe(pipe1);

    if (fork() == 0) {

        dup2(pipe1[1], 1);
        close(pipe1[0]);
        close(pipe1[1]);

        int in_fd = open("In.txt", O_RDONLY);
        dup2(in_fd, 0);
        close(in_fd);

        execlp("rev", "rev", NULL);
    }

    if (fork() == 0) {
        
        dup2(pipe1[0], 0);
        close(pipe1[1]);

        pipe(pipe1); 
        dup2(pipe1[1], 1);
        close(pipe1[0]);
        close(pipe1[1]);

        execlp("rev", "rev", NULL);
    }

    if (fork() == 0) {

        dup2(pipe1[0], 0);
        close(pipe1[1]);

        int in_fd = open("In.txt", O_RDONLY);
        dup2(in_fd, 0);
        close(in_fd);

        execlp("diff", "diff", "-", "In.txt", "-s", NULL);

    }

    close(pipe1[1]);

    while (wait(NULL) > 0);

    return 0;
}
