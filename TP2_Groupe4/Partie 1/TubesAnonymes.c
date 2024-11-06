// // // Ajouter les directives d'inclusion nécessaires
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main() {
//     int pipe1[2], pipe2[2];
    
//     // Création des tubes (pipes)
//     if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
//         perror("Erreur de création de pipe");
//         exit(EXIT_FAILURE);
//     }

//     // Premier processus fils pour `rev < In.txt`
//     pid_t pid1 = fork();
//     if (pid1 == 0) {
//         // Rediriger la sortie standard vers pipe1[1]
//         dup2(pipe1[1], 0);
//         close(pipe1[0]);  // Fermer le côté lecture de pipe1
//         close(pipe1[1]);  // Fermer le côté écriture de pipe1 après duplication
        
//         // Ouvrir In.txt comme entrée standard
//         FILE *input_file = fopen("In.txt", "r");
//         if (!input_file) {
//             perror("Erreur d'ouverture de In.txt");
//             exit(EXIT_FAILURE);
//         }
//         dup2(fileno(input_file), 0);
//         fclose(input_file);

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Deuxième processus fils pour `rev` (inverser à nouveau)
//     pid_t pid2 = fork();
//     if (pid2 == 0) {
//         // Rediriger l'entrée standard vers pipe1[0] et la sortie standard vers pipe2[1]
//         dup2(pipe1[0], 0);
//         dup2(pipe2[1], 1);

//         close(pipe1[0]);
//         close(pipe1[1]);
//         close(pipe2[0]);
//         close(pipe2[1]);

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Troisième processus fils pour `diff - In.txt -s`
//     pid_t pid3 = fork();
//     if (pid3 == 0) {
//         // Rediriger l'entrée standard vers pipe2[0]
//         dup2(pipe2[0], 0);
//         close(pipe1[0]);
//         close(pipe1[1]);
//         close(pipe2[0]);
//         close(pipe2[1]);

//         execlp("diff", "diff", "-", "In.txt", "-s", NULL);
//         perror("Erreur exec diff");
//         exit(EXIT_FAILURE);
//     }

//     // Fermeture des descripteurs de fichiers inutiles dans le processus principal
//     close(pipe1[0]);
//     close(pipe1[1]);
//     close(pipe2[0]);
//     close(pipe2[1]);

//     while(wait(NULL) > 0);

//     return EXIT_SUCCESS;
// }




// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main() {
//     int pipe1[2], pipe2[2];
    
    
//     if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
//         perror("Erreur de création de pipe");
//         exit(EXIT_FAILURE);
//     }

//     // Premier processus fils pour `rev < In.txt`
//     pid_t pid1 = fork();
//     if (pid1 == 0) {
        
//         dup2(pipe1[1], STDOUT_FILENO);
//         close(pipe1[0]);  
//         close(pipe1[1]);  
        
        
//         FILE *input_file = fopen("In.txt", "r");
//         if (!input_file) {
//             perror("Erreur d'ouverture de In.txt");
//             exit(EXIT_FAILURE);
//         }
//         dup2(fileno(input_file), STDIN_FILENO);
//         fclose(input_file);

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Processus intermédiaire pour afficher le texte inversé
//     pid_t pid_inter = fork();
//     if (pid_inter == 0) {
        
//         dup2(pipe1[0], STDIN_FILENO);
//         dup2(pipe2[1], STDOUT_FILENO);
//         close(pipe1[1]);
//         close(pipe2[0]);
        
//         char buffer[1024];
//         ssize_t n;
        
       
//         while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
//             write(STDOUT_FILENO, buffer, n);  
//             write(pipe2[1], buffer, n);       
//         }

//         close(pipe1[0]);
//         close(pipe2[1]);
//         exit(EXIT_SUCCESS);
//     }

//     // Deuxième processus fils pour `rev` (inverser à nouveau)
//     pid_t pid2 = fork();
//     if (pid2 == 0) {
        
//         dup2(pipe2[0], STDIN_FILENO);
//         close(pipe2[1]);
//         close(pipe1[0]);
//         close(pipe1[1]);

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Troisième processus fils pour `diff - In.txt -s`
//     pid_t pid3 = fork();
//     if (pid3 == 0) {

//         dup2(pipe1[0], STDIN_FILENO);
//         close(pipe1[0]);
//         close(pipe1[1]);
//         close(pipe2[0]);
//         close(pipe2[1]);

//         execlp("diff", "diff", "-", "In.txt", "-s", NULL);
//         perror("Erreur exec diff");
//         exit(EXIT_FAILURE);
//     }

//     close(pipe1[0]);
//     close(pipe1[1]);
//     close(pipe2[0]);
//     close(pipe2[1]);

//     while(wait(NULL) > 0);

//     return EXIT_SUCCESS;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/wait.h>

// int main() {
//     int pipe1[2], pipe2[2];
    
//     // Création des tubes (pipes)
//     if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
//         perror("Erreur de création de pipe");
//         exit(EXIT_FAILURE);
//     }

//     // Premier processus fils pour `rev < In.txt`
//     pid_t pid1 = fork();
//     if (pid1 == 0) {
//         // Rediriger la sortie standard vers pipe1[1]
//         dup2(pipe1[1], 1);  // Remarque: redirection de sortie, pas d'entrée
//         close(pipe1[0]);     // Fermer le côté lecture de pipe1
//         close(pipe1[1]);     // Fermer le côté écriture de pipe1 après duplication
        
//         // Ouvrir In.txt en lecture seule et le rediriger vers l'entrée standard
//         int input_fd = open("In.txt", O_RDONLY);
//         if (input_fd == -1) {
//             perror("Erreur d'ouverture de In.txt");
//             exit(EXIT_FAILURE);
//         }
//         dup2(input_fd, 0);   // Redirige l'entrée standard vers input_fd
//         close(input_fd);      // Fermer input_fd après duplication

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Deuxième processus fils pour `rev` (inverser à nouveau)
//     pid_t pid2 = fork();
//     if (pid2 == 0) {
//         // Rediriger l'entrée standard vers pipe1[0] et la sortie standard vers pipe2[1]
//         dup2(pipe1[0], 0);
//         dup2(pipe2[1], 1);

//         close(pipe1[0]);
//         close(pipe1[1]);
//         close(pipe2[0]);
//         close(pipe2[1]);

//         execlp("rev", "rev", NULL);
//         perror("Erreur exec rev");
//         exit(EXIT_FAILURE);
//     }

//     // Troisième processus fils pour `diff - In.txt -s`
//     pid_t pid3 = fork();
//     if (pid3 == 0) {
//         // Rediriger l'entrée standard vers pipe2[0]
//         dup2(pipe2[0], 0);
//         close(pipe1[0]);
//         close(pipe1[1]);
//         close(pipe2[0]);
//         close(pipe2[1]);

//         execlp("diff", "diff", "-", "In.txt", "-s", NULL);
//         perror("Erreur exec diff");
//         exit(EXIT_FAILURE);
//     }

//     // Fermeture des descripteurs de fichiers inutiles dans le processus principal
//     close(pipe1[0]);
//     close(pipe1[1]);
//     close(pipe2[0]);
//     close(pipe2[1]);

//     // Attendre la fin de tous les processus fils
//     while (wait(NULL) > 0);

//     return EXIT_SUCCESS;
// }



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe1[2], pipe2[2];
    char buffer[BUFFER_SIZE];
    ssize_t nbytes;

    // Création des pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Erreur de création de pipe");
        exit(EXIT_FAILURE);
    }

    // Premier processus fils pour `rev < In.txt`
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Rediriger la sortie standard vers pipe1[1]
        dup2(pipe1[1], 1);
        close(pipe1[0]);
        close(pipe1[1]);
        
        // Ouvrir In.txt en lecture seule et le rediriger vers l'entrée standard
        int input_fd = open("In.txt", O_RDONLY);
        if (input_fd == -1) {
            perror("Erreur d'ouverture de In.txt");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, 0);
        close(input_fd);

        execlp("rev", "rev", NULL);
        perror("Erreur exec rev");
        exit(EXIT_FAILURE);
    }

    // Attendre que le premier `rev` termine et lire le résultat dans pipe1
    close(pipe1[1]);  // Fermer l'écriture de pipe1 dans le processus parent
    nbytes = read(pipe1[0], buffer, BUFFER_SIZE);
    buffer[nbytes] = '\0';  // Marquer la fin de chaîne pour l'affichage
    printf("Sortie après le premier rev:\n%s\n", buffer);
    close(pipe1[0]);

    // Création du deuxième processus fils pour `rev` (inverser à nouveau)
    if (pipe(pipe1) == -1) {  // Réutiliser pipe1 pour le second rev
        perror("Erreur de création de pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Rediriger l'entrée standard vers pipe1[0] et la sortie standard vers pipe2[1]
        dup2(pipe1[0], 0);
        dup2(pipe2[1], 1);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("rev", "rev", NULL);
        perror("Erreur exec rev");
        exit(EXIT_FAILURE);
    }

    // Écrire la sortie du premier rev dans le pipe1 pour le deuxième processus
    close(pipe1[0]);  // Fermer la lecture de pipe1 dans le processus parent
    write(pipe1[1], buffer, nbytes);
    close(pipe1[1]);

    // Attendre que le deuxième `rev` termine et lire le résultat dans pipe2
    close(pipe2[1]);  // Fermer l'écriture de pipe2 dans le processus parent
    nbytes = read(pipe2[0], buffer, BUFFER_SIZE);
    buffer[nbytes] = '\0';
    printf("Sortie après le deuxième rev (retour à l'original):\n%s\n", buffer);
    close(pipe2[0]);

    // Troisième processus fils pour `diff - In.txt -s`
    pid_t pid3 = fork();
    if (pid3 == 0) {
        // Rediriger l'entrée standard vers pipe2[0]
        int input_fd = open("In.txt", O_RDONLY);
        dup2(pipe2[0], 0);
        close(pipe2[0]);
        close(pipe2[1]);
        dup2(input_fd, 0);
        execlp("diff", "diff", "-", "In.txt", "-s", NULL);
        perror("Erreur exec diff");
        exit(EXIT_FAILURE);
    }

    // Attendre la fin de tous les processus fils
    while (wait(NULL) > 0);

    return EXIT_SUCCESS;
}

