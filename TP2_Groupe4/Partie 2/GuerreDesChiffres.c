/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * GuerreDesChiffres.c
 * Ghobrial, Mina Youhanna Helmi / 2214988
 * Rouabah, Abdelmounaim / 2211513
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

int index_prod = 0;
int index_cons = 0;
int total_produit = 0;
int total_consomme = 0;
int count_produit = 0;
int count_consomme = 0;
int nb_producteurs, nb_consommateurs, taille_du_buffer;
int *buffer;
sem_t mutex, vide, plein;
volatile sig_atomic_t fin = 0;

void* fonction_producteur(void* pid) {
    int id = *(int *)pid;
    while (1) {
        int item = (rand() % 9) + 1;
        sem_wait(&vide);
        sem_wait(&mutex);
        
        buffer[index_prod] = item;
        index_prod = (index_prod + 1) % taille_du_buffer;
        
        total_produit += item;
        count_produit++;
        
        sem_post(&mutex);
        sem_post(&plein);
        
        if (fin) break;
    }
    pthread_exit(NULL);
}

void* fonction_consommateur(void* cid) {
    //je ne sais pas sert a quoi ?!
    // int id = *(int *)cid;
    
    while (1) {
        sem_wait(&plein);
        sem_wait(&mutex);

        int item = buffer[index_cons];
        index_cons = (index_cons + 1) % taille_du_buffer;

        if (item == 0) {
            sem_post(&mutex);
            break;
        }
        
        total_consomme += item;
        count_consomme++;
        
        sem_post(&mutex);
        sem_post(&vide);
    }
    pthread_exit(NULL);
}

void signal_alarme(int sig) {
    fin = 1;
}

int main(int argc, char* argv[]) {
    nb_producteurs = atoi(argv[1]);
    nb_consommateurs = atoi(argv[2]);
    taille_du_buffer = atoi(argv[3]);
    buffer = (int *)malloc(taille_du_buffer * sizeof(int));

    srand(time(NULL));
    
    sem_init(&mutex, 0, 1);
    sem_init(&vide, 0, taille_du_buffer);
    sem_init(&plein, 0, 0);

    pthread_t producteurs[nb_producteurs];
    int id_producteurs[nb_producteurs];
    for (int i = 0; i < nb_producteurs; i++) {
        id_producteurs[i] = i;
        pthread_create(&producteurs[i], NULL, fonction_producteur, &id_producteurs[i]);
    }

    pthread_t consommateurs[nb_consommateurs];
    int id_consommateurs[nb_consommateurs];
    for (int i = 0; i < nb_consommateurs; i++) {
        id_consommateurs[i] = i;
        pthread_create(&consommateurs[i], NULL, fonction_consommateur, &id_consommateurs[i]);
    }

    signal(SIGALRM, signal_alarme);
    alarm(1); 

    for (int i = 0; i < nb_producteurs; i++) {
        pthread_join(producteurs[i], NULL);
    }

    for (int i = 0; i < nb_consommateurs; i++) {
        sem_wait(&vide);
        sem_wait(&mutex);
        buffer[index_prod] = 0;
        index_prod = (index_prod + 1) % taille_du_buffer;
        sem_post(&mutex);
        sem_post(&plein);
    }

    for (int i = 0; i < nb_consommateurs; i++) {
        pthread_join(consommateurs[i], NULL);
    }

    printf("total de chiffres produits : %d\n", total_produit);
    printf("total de chiffres consom : %d\n", total_consomme);
    printf("nombre de chiffres produits : %d\n", count_produit);
    printf("nombre de chiffres consom : %d\n", count_consomme);

    free(buffer);
    sem_destroy(&mutex);
    sem_destroy(&vide);
    sem_destroy(&plein);

    return 0;
}