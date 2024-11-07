// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include <sys/stat.h>

// #define FIFO_PATH "tube_nommee"

// int main() {
//     // Supprimer le tube nommé s'il existe déjà, pour éviter des conflits
//     unlink(FIFO_PATH);

//     // Création du tube nommé avec des permissions de lecture-écriture pour tous
//     if (mkfifo(FIFO_PATH, 0666) == -1) {
//         perror("Erreur : impossible de créer le tube nommé");
//         exit(EXIT_FAILURE);
//     }

//     // Premier processus fils pour `rev < In.txt`
//     pid_t pid1 = fork();
//     if (pid1 == 0) {
//         // Ouverture du fichier In.txt en lecture seule pour le rediriger vers l'entrée standard
//         int input_fd = open("In.txt", O_RDONLY);
//         if (input_fd == -1) {
//             perror("Erreur : impossible d'ouvrir In.txt");
//             exit(EXIT_FAILURE);
//         }
//         dup2(input_fd, STDIN_FILENO);
//         close(input_fd);

//         // Ouverture du tube nommé en écriture seule et redirection vers la sortie standard
//         int fifo_fd = open(FIFO_PATH, O_WRONLY);
//         if (fifo_fd == -1) {
//             perror("Erreur : impossible d'ouvrir le tube nommé en écriture");
//             exit(EXIT_FAILURE);
//         }
//         dup2(fifo_fd, STDOUT_FILENO);
//         close(fifo_fd);

//         // Exécution de la commande rev
//         execlp("rev", "rev", NULL);
//         perror("Erreur : execlp rev a échoué");
//         exit(EXIT_FAILURE);
//     }

//     // Deuxième processus fils pour `rev` (inversion supplémentaire)
//     pid_t pid2 = fork();
//     if (pid2 == 0) {
//         // Ouverture du tube nommé en lecture seule et redirection vers l'entrée standard
//         int fifo_fd = open(FIFO_PATH, O_RDONLY);
//         if (fifo_fd == -1) {
//             perror("Erreur : impossible d'ouvrir le tube nommé en lecture");
//             exit(EXIT_FAILURE);
//         }
//         dup2(fifo_fd, 0);
//         close(fifo_fd);

//         // Exécution de la commande rev
//         execlp("rev", "rev", NULL);
//         perror("Erreur : execlp rev a échoué");
//         exit(EXIT_FAILURE);
//     }

//     // Troisième processus fils pour `diff - In.txt -s`
//     pid_t pid3 = fork();
//     if (pid3 == 0) {
//         // Ouverture du tube nommé en lecture seule pour la comparaison finale
//         int fifo_fd = open(FIFO_PATH, O_RDONLY);
//         if (fifo_fd == -1) {
//             perror("Erreur : impossible d'ouvrir le tube nommé en lecture");
//             exit(EXIT_FAILURE);
//         }
//         dup2(fifo_fd, 0);
//         close(fifo_fd);

//         // Exécution de la commande diff pour comparer avec In.txt
//         execlp("diff", "diff", "-", "In.txt", "-s", NULL);
//         perror("Erreur : execlp diff a échoué");
//         exit(EXIT_FAILURE);
//     }

//     // Fermeture des descripteurs inutiles dans le processus principal
//     // (aucun descripteur supplémentaire à fermer ici)

//     // Attente de la fin de tous les processus fils
//     waitpid(pid1, NULL, 0);
//     waitpid(pid2, NULL, 0);
//     waitpid(pid3, NULL, 0);

//     // Suppression du tube nommé après l'exécution
//     unlink(FIFO_PATH);

//     return EXIT_SUCCESS;
// }



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH_WRITE "tube_nommee_write"
#define FIFO_PATH_READ "tube_nommee_read"

int main() {
    ///sil existe
    unlink(FIFO_PATH_WRITE);
    unlink(FIFO_PATH_READ);

    mkfifo(FIFO_PATH_WRITE, 0666);
    mkfifo(FIFO_PATH_READ, 0666);

    if (fork() == 0) {
        int input_fd = open("In.txt", O_RDONLY);
        int fifo_write_fd = open(FIFO_PATH_WRITE, O_WRONLY);

        dup2(input_fd, 0);
        dup2(fifo_write_fd, 1);
        close(input_fd);
        close(fifo_write_fd);

        execlp("rev", "rev", NULL);
    }

    if (fork() == 0) {
        int fifo_read_fd = open(FIFO_PATH_WRITE, O_RDONLY);
        int fifo_write_fd = open(FIFO_PATH_READ, O_WRONLY);

        dup2(fifo_read_fd, 0);
        dup2(fifo_write_fd, 1);
        close(fifo_read_fd);
        close(fifo_write_fd);

        execlp("rev", "rev", NULL);

    }

    if (fork() == 0) {
        int fifo_read_fd = open(FIFO_PATH_READ, O_RDONLY);
        dup2(fifo_read_fd, 0);
        close(fifo_read_fd);

        execlp("diff", "diff", "-", "In.txt", "-s", NULL);

    }

    while(wait(NULL) > 0);

    unlink(FIFO_PATH_WRITE);
    unlink(FIFO_PATH_READ);

    return 0;
}
