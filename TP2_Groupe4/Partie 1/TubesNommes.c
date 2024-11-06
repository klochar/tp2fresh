#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "tube_nommee"

int main() {
    // Supprimer le tube nommé s'il existe déjà, pour éviter des conflits
    unlink(FIFO_PATH);

    // Création du tube nommé avec des permissions de lecture-écriture pour tous
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Erreur : impossible de créer le tube nommé");
        exit(EXIT_FAILURE);
    }

    // Premier processus fils pour `rev < In.txt`
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Ouverture du fichier In.txt en lecture seule pour le rediriger vers l'entrée standard
        int input_fd = open("In.txt", O_RDONLY);
        if (input_fd == -1) {
            perror("Erreur : impossible d'ouvrir In.txt");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);

        // Ouverture du tube nommé en écriture seule et redirection vers la sortie standard
        int fifo_fd = open(FIFO_PATH, O_WRONLY);
        if (fifo_fd == -1) {
            perror("Erreur : impossible d'ouvrir le tube nommé en écriture");
            exit(EXIT_FAILURE);
        }
        dup2(fifo_fd, STDOUT_FILENO);
        close(fifo_fd);

        // Exécution de la commande rev
        execlp("rev", "rev", NULL);
        perror("Erreur : execlp rev a échoué");
        exit(EXIT_FAILURE);
    }

    // Deuxième processus fils pour `rev` (inversion supplémentaire)
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Ouverture du tube nommé en lecture seule et redirection vers l'entrée standard
        int fifo_fd = open(FIFO_PATH, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Erreur : impossible d'ouvrir le tube nommé en lecture");
            exit(EXIT_FAILURE);
        }
        dup2(fifo_fd, 0);
        close(fifo_fd);

        // Exécution de la commande rev
        execlp("rev", "rev", NULL);
        perror("Erreur : execlp rev a échoué");
        exit(EXIT_FAILURE);
    }

    // Troisième processus fils pour `diff - In.txt -s`
    pid_t pid3 = fork();
    if (pid3 == 0) {
        // Ouverture du tube nommé en lecture seule pour la comparaison finale
        int fifo_fd = open(FIFO_PATH, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Erreur : impossible d'ouvrir le tube nommé en lecture");
            exit(EXIT_FAILURE);
        }
        dup2(fifo_fd, 0);
        close(fifo_fd);

        // Exécution de la commande diff pour comparer avec In.txt
        execlp("diff", "diff", "-", "In.txt", "-s", NULL);
        perror("Erreur : execlp diff a échoué");
        exit(EXIT_FAILURE);
    }

    // Fermeture des descripteurs inutiles dans le processus principal
    // (aucun descripteur supplémentaire à fermer ici)

    // Attente de la fin de tous les processus fils
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    // Suppression du tube nommé après l'exécution
    unlink(FIFO_PATH);

    return EXIT_SUCCESS;
}