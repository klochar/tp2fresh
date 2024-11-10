/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * TubesNommes.c
 * Ghobrial, Mina Youhanna Helmi / 2214988
 * Rouabah, Abdelmounaim / 2211513
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    const char *fifo_write = "tube_nommee_write";
    const char *fifo_read = "tube_nommee_read";

    mkfifo(fifo_write, 0666);
    mkfifo(fifo_read, 0666);

    if (fork() == 0) {
        int input_fd = open("In.txt", O_RDONLY);
        int fifo_write_fd = open(fifo_write, O_WRONLY);

        dup2(input_fd, 0);
        dup2(fifo_write_fd, 1);
        close(input_fd);
        close(fifo_write_fd);

        execlp("rev", "rev", NULL);
    }

    if (fork() == 0) {
        int fifo_read_fd = open(fifo_write, O_RDONLY);
        int fifo_write_fd = open(fifo_read, O_WRONLY);

        dup2(fifo_read_fd, 0);
        dup2(fifo_write_fd, 1);
        close(fifo_read_fd);
        close(fifo_write_fd);

        execlp("rev", "rev", NULL);

    }

    if (fork() == 0) {
        int fifo_read_fd = open(fifo_read, O_RDONLY);
        dup2(fifo_read_fd, 0);
        close(fifo_read_fd);

        execlp("diff", "diff", "-", "In.txt", "-s", NULL);

    }

    while(wait(NULL) > 0);

    unlink(fifo_write);
    unlink(fifo_read);

    return 0;
}

