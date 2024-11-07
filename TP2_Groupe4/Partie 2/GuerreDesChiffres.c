// #include <stdio.h>
// //..

// void* producteur(void* pid) {
//     // ...
//     return NULL;
// }

// void* consommateur(void* cid) {
//     // ...

//     return NULL;
// }
// // ...

// int main(int argc, char* argv[]) {
//     // Les paramètres du programme sont, dans l'ordre :
//     // le nombre de producteurs, le nombre de consommateurs
//     // et la taille du tampon.

//     // ..
//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h>
// #include <signal.h>
// #include <stdbool.h>

// #define BUFFER_SIZE 10 // Taille par défaut du tampon

// // Variables globales
// int buffer[BUFFER_SIZE];  // Tampon circulaire
// int in = 0;               // Indice d'insertion
// int out = 0;              // Indice de retrait
// int total_produced = 0;   // Total des chiffres produits
// int total_consumed = 0;   // Total des chiffres consommés
// int sum_produced = 0;     // Somme des chiffres produits
// int sum_consumed = 0;     // Somme des chiffres consommés
// bool flag_de_fin = false;  // Indicateur pour terminer les threads

// // Sémaphores
// sem_t empty; // Compte les espaces vides dans le tampon
// sem_t full;  // Compte les éléments présents dans le tampon
// pthread_mutex_t mutex; // Mutex pour protéger l'accès au tampon

// // Fonction de traitement du signal
// void signal_handler(int signum) {
//     flag_de_fin = true; // Mettre fin aux producteurs
// }

// // Fonction producteur
// void *producteur(void *arg) {
//     int id = *((int *)arg);
//     free(arg); // Libérer la mémoire allouée pour le paramètre

//     while (true) {
//         int chiffre = rand() % 9 + 1; // Générer un chiffre aléatoire entre 1 et 9
//         sem_wait(&empty); // Attendre un espace vide
//         pthread_mutex_lock(&mutex); // Protéger l'accès au tampon

//         buffer[in] = chiffre; // Ajouter le chiffre au tampon
//         in = (in + 1) % BUFFER_SIZE; // Mettre à jour l'indice d'insertion
//         total_produced++;
//         sum_produced += chiffre;

//         printf("Producteur %d a produit: %d\n", id, chiffre);
//         pthread_mutex_unlock(&mutex); // Libérer le mutex
//         sem_post(&full); // Indiquer qu'il y a un élément de plus dans le tampon

//         // Vérifier si l'on doit terminer
//         if (flag_de_fin) {
//             break;
//         }
//     }

//     // Communiquer la somme produite au thread principal
//     pthread_exit((void *)(long)sum_produced);
// }

// // Fonction consommateur
// void *consommateur(void *arg) {
//     int id = *((int *)arg);
//     free(arg); // Libérer la mémoire allouée pour le paramètre

//     while (true) {
//         sem_wait(&full); // Attendre qu'il y ait des éléments dans le tampon
//         pthread_mutex_lock(&mutex); // Protéger l'accès au tampon

//         int chiffre = buffer[out]; // Retirer un chiffre du tampon
//         out = (out + 1) % BUFFER_SIZE; // Mettre à jour l'indice de retrait
//         total_consumed++;
//         sum_consumed += chiffre;

//         printf("Consommateur %d a consommé: %d\n", id, chiffre);
//         pthread_mutex_unlock(&mutex); // Libérer le mutex
//         sem_post(&empty); // Indiquer qu'il y a un espace vide

//         // Vérifier si le chiffre est 0 pour terminer
//         if (chiffre == 0) {
//             break;
//         }
//     }

//     // Communiquer la somme consommée au thread principal
//     pthread_exit((void *)(long)sum_consumed);
// }

// int main(int argc, char *argv[]) {
//     if (argc != 4) {
//         fprintf(stderr, "Usage: %s <nombre_producteurs> <nombre_consommateurs> <taille_tampon>\n", argv[0]);
//         return EXIT_FAILURE;
//     }

//     int num_producteurs = atoi(argv[1]);
//     int num_consommateurs = atoi(argv[2]);
//     int taille_tampon = atoi(argv[3]);
    
//     // Initialiser le tampon de taille spécifiée
//     if (taille_tampon > BUFFER_SIZE) {
//         fprintf(stderr, "Taille du tampon doit être <= %d\n", BUFFER_SIZE);
//         return EXIT_FAILURE;
//     }

//     // Initialiser les sémaphores et le mutex
//     sem_init(&empty, 0, taille_tampon); // Initialise avec la taille du tampon
//     sem_init(&full, 0, 0); // Aucun élément au début
//     pthread_mutex_init(&mutex, NULL); // Initialiser le mutex

//     // Gérer le signal SIGALRM
//     signal(SIGALRM, signal_handler);
//     alarm(1); // Déclencher l'alarme dans 1 seconde

//     // Création des threads producteurs
//     pthread_t producteurs[num_producteurs];
//     for (int i = 0; i < num_producteurs; i++) {
//         int *id = malloc(sizeof(int)); // Allouer de la mémoire pour l'identifiant
//         *id = i;
//         pthread_create(&producteurs[i], NULL, producteur, id);
//     }

//     // Création des threads consommateurs
//     pthread_t consommateurs[num_consommateurs];
//     for (int i = 0; i < num_consommateurs; i++) {
//         int *id = malloc(sizeof(int)); // Allouer de la mémoire pour l'identifiant
//         *id = i;
//         pthread_create(&consommateurs[i], NULL, consommateur, id);
//     }

//     // Attendre la fin de tous les producteurs
//     for (int i = 0; i < num_producteurs; i++) {
//         void *res;
//         pthread_join(producteurs[i], &res);
//         printf("Somme produite par le producteur %d: %ld\n", i, (long)res);
//     }

//     // Déposer des chiffres 0 dans le tampon pour signaler la fin aux consommateurs
//     for (int i = 0; i < num_consommateurs; i++) {
//         sem_wait(&empty);
//         pthread_mutex_lock(&mutex);
//         buffer[in] = 0; // Ajouter un 0 au tampon
//         in = (in + 1) % taille_tampon;
//         pthread_mutex_unlock(&mutex);
//         sem_post(&full);
//     }

//     // Attendre la fin de tous les consommateurs
//     for (int i = 0; i < num_consommateurs; i++) {
//         void *res;
//         pthread_join(consommateurs[i], &res);
//         printf("Somme consommée par le consommateur %d: %ld\n", i, (long)res);
//     }

//     // Afficher les résultats
//     printf("Total produit: %d, Total consommé: %d\n", total_produced, total_consumed);
//     printf("Somme des chiffres produits: %d, Somme des chiffres consommés: %d\n", sum_produced, sum_consumed);

//     // Nettoyer
//     sem_destroy(&empty);
//     sem_destroy(&full);
//     pthread_mutex_destroy(&mutex);

//     return EXIT_SUCCESS;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <semaphore.h>
// #include <signal.h>


// int entre_buffer = 0, sortie_buffer = 0;
// int somme_produits = 0, somme_consom = 0;
// int chiffres_produits = 0, chiffres_consommes = 0;
// int nmbProd, nmbCons,taille_buffer;
// int *buffer;
// sem_t mutex, empty, full;
// volatile sig_atomic_t flag_de_fin = 0;

// void* producteur(void* pid) {
//       int id = *(int *)pid;
//     while (1) {
//         int x = (rand() % 9) + 1; // Générer aléatoirement un chiffre non nul
//         sem_wait(&empty); // Attendre qu'il y ait de la place dans le tampon
//         sem_wait(&mutex); // Entrée en section critique
//         buffer[entre_buffer] = x; // Déposer le chiffre dans le tampon
//         entre_buffer = (entre_buffer + 1) % taille_buffer; // Mettre à jour l'indice du tampon circulaire
//         somme_produits += x; // Mettre à jour la somme des chiffres produits
//         chiffres_produits++;
//         sem_post(&mutex); // Sortie de section critique
//         sem_post(&full); // Indiquer qu'il y a un élément de plus dans le tampon
//         if (flag_de_fin) break; // Vérifier si le traitement doit être interrompu
//     }
//     pthread_exit(NULL);
// }

// void* consommateur(void* cid) {
//      int id = *(int *)cid;
//     while (1) {
//         sem_wait(&full); // Attendre qu'il y ait des éléments dans le tampon
//         sem_wait(&mutex); // Entrée en section critique
//         int x = buffer[sortie_buffer]; // Récupérer un chiffre du tampon
//         sortie_buffer = (sortie_buffer + 1) % taille_buffer; // Mettre à jour l'indice du tampon circulaire
//         if (x == 0) break; // Vérifier si le chiffre est 0
//         somme_consom += x; // Mettre à jour la somme des chiffres consommés
//         chiffres_consommes++;
//         sem_post(&mutex); // Sortie de section critique
//         sem_post(&empty); // Indiquer qu'il y a un espace de plus dans le tampon
//     }
//     sem_post(&mutex); // Sortie de section critique (au cas où il y aurait un interblocage)
//     pthread_exit(NULL);
// }

// alarme(){
//      flag_de_fin = 1; 
// }

// int main(int argc, char* argv[]) {
//     // Les paramètres du programme sont, dans l'ordre :
//     // le nombre de producteurs, le nombre de consommateurs
//     // et la taille du tampon.
//     printf("MAIN: \n");
//     nmbProd = atoi(argv[1]);
//     nmbCons = atoi(argv[2]);
//     taille_buffer = atoi(argv[3]);
//     buffer = (int *)malloc(taille_buffer * sizeof(int));


//     srand(time(NULL));

//     // Initialisation des sémaphores
//     sem_init(&mutex, 0, 1); // Mutex pour la section critique
//     sem_init(&empty, 0, taille_buffer); // Nombre de places vides dans le tampon
//     sem_init(&full, 0, 0); // Nombre d'éléments dans le tampon

//     // Création des threads producteurs
//     pthread_t producers[nmbProd];
//     int producer_ids[nmbProd];
//     for (int i = 0; i < nmbProd; i++) {
//         producer_ids[i] = i;
//         printf("phtreadCReate prod: \n");
//         pthread_create(&producers[i], NULL, producteur, (void *)&producer_ids[i]);
//     }

//     // Création des threads consommateurs
//     pthread_t consumers[nmbCons];
//     int consumer_ids[nmbCons];
//     for (int i = 0; i < nmbCons; i++) {
//         consumer_ids[i] = i;
//         printf("phtreadCReate cons: \n");
//         pthread_create(&consumers[i], NULL, consommateur, (void *)&consumer_ids[i]);
//     }

//     signal(SIGALRM, alarme);
//     alarm(1); // Armer l'alarme pour 1 seconde

//     // Attendre la fin des threads producteurs
//     for (int i = 0; i < nmbProd; i++) {
//         pthread_join(producers[i], NULL);
//     }

//     // Déposer autant de chiffres 0 qu'il y a de threads consommateurs
//     for (int i = 0; i < nmbCons; i++) {
//         sem_wait(&empty);
//         sem_wait(&mutex);
//         buffer[entre_buffer] = 0;
//         entre_buffer = (entre_buffer + 1) % taille_buffer;
//         sem_post(&mutex);
//         sem_post(&full);
//     }

//     // Attendre la fin des threads consommateurs
//     for (int i = 0; i < nmbCons; i++) {
//         pthread_join(consumers[i], NULL);
//     }

//     // Afficher les sommes et les nombres de chiffres produits et consommés
//     printf("Somme des chiffres produits: %d\n", somme_produits);
//     printf("Somme des chiffres consommés: %d\n", somme_consom);
//     printf("Nombre de chiffres produits: %d\n", chiffres_produits);
//     printf("Nombre de chiffres consommés: %d\n", chiffres_consommes);

//     // Destruction des sémaphores
//     sem_destroy(&mutex);
//     sem_destroy(&empty);
//     sem_destroy(&full);

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

int entre_buffer = 0;
int sortie_buffer = 0;
int somme_produits = 0;
int somme_consom = 0;
int chiffres_produits = 0;
int chiffres_consommes = 0;
int nmbProd, nmbCons, taille_buffer;
int *buffer;
sem_t mutex, empty, full;
volatile sig_atomic_t flag_de_fin = 0;

void* producteur(void* pid) {
    int id = *(int *)pid;
    while (1) {
        int x = (rand() % 9) + 1; 
        sem_wait(&empty); 
        sem_wait(&mutex); 
        
        buffer[entre_buffer] = x;
        entre_buffer = (entre_buffer + 1) % taille_buffer;
        
        somme_produits += x;
        chiffres_produits++;
        
        sem_post(&mutex); 
        sem_post(&full); 
        
        if (flag_de_fin) break; 
    }
    pthread_exit(NULL);
}

void* consommateur(void* cid) {
    int id = *(int *)cid;
    while (1) {
        sem_wait(&full); 
        sem_wait(&mutex); 

        int x = buffer[sortie_buffer]; 
        sortie_buffer = (sortie_buffer + 1) % taille_buffer;

        if (x == 0) { 
            sem_post(&mutex);
            break;
        }
        
        somme_consom += x;
        chiffres_consommes++;
        
        sem_post(&mutex); 
        sem_post(&empty);
    }
    pthread_exit(NULL);
}

void alarme(int sig) {
    flag_de_fin = 1;
}

int main(int argc, char* argv[]) {
    // if (argc < 4) {
    //     fprintf(stderr, "Usage: %s <nb producteurs> <nb consommateurs> <taille buffer>\n", argv[0]);
    //     exit(EXIT_FAILURE);
    // }

    nmbProd = atoi(argv[1]);
    nmbCons = atoi(argv[2]);
    taille_buffer = atoi(argv[3]);
    buffer = (int *)malloc(taille_buffer * sizeof(int));

    srand(time(NULL));
    
    sem_init(&mutex, 0, 1);        
    sem_init(&empty, 0, taille_buffer); 
    sem_init(&full, 0, 0);         

    pthread_t producers[nmbProd];
    int producer_ids[nmbProd];
    for (int i = 0; i < nmbProd; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producteur, &producer_ids[i]);
        // if (pthread_create(&producers[i], NULL, producteur, &producer_ids[i]) != 0) {
        //     perror("Erreur lors de la création d'un thread producteur");
        //     exit(EXIT_FAILURE);
        // }
    }

    pthread_t consumers[nmbCons];
    int consumer_ids[nmbCons];
    for (int i = 0; i < nmbCons; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consommateur, &consumer_ids[i]);
        // if (pthread_create(&consumers[i], NULL, consommateur, &consumer_ids[i]) != 0) {
        //     perror("Erreur lors de la création d'un thread consommateur");
        //     exit(EXIT_FAILURE);
        // }
    }

    signal(SIGALRM, alarme);
    alarm(1); 

    for (int i = 0; i < nmbProd; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < nmbCons; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);
        buffer[entre_buffer] = 0;
        entre_buffer = (entre_buffer + 1) % taille_buffer;
        sem_post(&mutex);
        sem_post(&full);
    }

    for (int i = 0; i < nmbCons; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("Somme des chiffres produits: %d\n", somme_produits);
    printf("Somme des chiffres consumed: %d\n", somme_consom);
    printf("Nombre de chiffres produits: %d\n", chiffres_produits);
    printf("Nombre de chiffres consumed: %d\n", chiffres_consommes);

    free(buffer);
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
