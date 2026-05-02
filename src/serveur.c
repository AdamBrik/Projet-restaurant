#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "../include/protocole.h"

#define TAILLE_FILE 10 
#define nb_thread 3 

Commande file_attente[TAILLE_FILE]; 
int premier = 0;      
int queue = 0;      
int nb_requetes = 0; 

pthread_mutex_t mutex_file; 
sem_t sem_requetes;         

char *tube_s_r_global;
char *fichier_config_global;

void *thread(void *arg) {
    int id_threads = *((int *)arg); 

    while (1) {
        sem_wait(&sem_requetes);

        pthread_mutex_lock(&mutex_file);

        Commande cmd = file_attente[queue];
        queue = (queue + 1) % TAILLE_FILE;
        nb_requetes--; 

        pthread_mutex_unlock(&mutex_file);
        
        Reponse rep;
        rep.id_client = cmd.id_client; 
        rep.statut = 0; 
        
        FILE *config = fopen(fichier_config_global, "r");
        char nom_resto[50], nom_fichier_menu[50];
        int id_lieu_config;
        int lieu_trouve = 0;

        if (config != NULL) {
            while(fscanf(config, "\"%[^\"]\" %d \"%[^\"]\"\n", nom_resto, &id_lieu_config, nom_fichier_menu) == 3) {
                if (id_lieu_config == cmd.numero_table) {
                    lieu_trouve = 1;
                    break;
                }
            }
            fclose(config);
        }

        if (lieu_trouve) {
            char nom_fichier_cible[50];
            snprintf(nom_fichier_cible, sizeof(nom_fichier_cible), "menu_%d.txt", cmd.id_plat);
            
            FILE *fichier_menu = fopen(nom_fichier_cible, "r");
            if (fichier_menu != NULL) {
                char menu_filtre[800] = ""; 
                char ligne[180];
                int au_moins_un_plat = 0;

                while (fgets(ligne, sizeof(ligne), fichier_menu) != NULL) {
                    int ignorer_plat = 0;
                    
                    if (strcmp(cmd.filtre, "aucun") != 0) {
                        if (strstr(ligne, cmd.filtre) != NULL) {
                            ignorer_plat = 1;
                        }
                    }

                    if (ignorer_plat == 0) {
                        strcat(menu_filtre, ligne);
                        au_moins_un_plat = 1;
                    }
                }
                
                if (au_moins_un_plat) {
                    if (strcmp(cmd.filtre, "aucun") == 0) {
                        snprintf(rep.message, sizeof(rep.message), "[%s] Menu complet :\n%s", nom_resto, menu_filtre);
                    } else {
                        snprintf(rep.message, sizeof(rep.message), "[%s] Menu (sans %s) :\n%s", nom_resto, cmd.filtre, menu_filtre);
                    }
                    rep.statut = 1;
                } else {
                    snprintf(rep.message, sizeof(rep.message), "Aucun plat ne correspond à vos critères.");
                }
                fclose(fichier_menu);
            } else {
                snprintf(rep.message, sizeof(rep.message), "[%s] Erreur: Le fichier %s est introuvable.", nom_resto, nom_fichier_cible);
            }
        } else {
            snprintf(rep.message, sizeof(rep.message), "Désolé, le lieu %d n'existe pas.", cmd.numero_table);
        }

        int fd_s_r = open(tube_s_r_global, O_WRONLY);
        if(fd_s_r != -1) {
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
            printf("[%d] Repas envoyé au client %d !\n", id_threads, rep.id_client);
        }
    }
    return NULL;
}



int main(int argc, char *argv[]) {
    if (argc != 4) {
        return EXIT_FAILURE;
    }

    char *tube_r_s = argv[1];
    tube_s_r_global = argv[2];       
    fichier_config_global = argv[3];

    pthread_mutex_init(&mutex_file, NULL);
    sem_init(&sem_requetes, 0, 0); 

    pthread_t threads[nb_thread];
    int id_threads[nb_thread]; 

    printf("Création de threads...\n");
    for (int i = 0; i < nb_thread; i++) {
        id_threads[i] = i + 1;
        pthread_create(&threads[i], NULL, thread, &id_threads[i]);
    }

    Commande cmd;
    while(1) {
        int fd_r_s = open(tube_r_s, O_RDONLY);
        if (fd_r_s == -1) continue;
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            if (!commande_est_valide(&cmd)) {
                fprintf(stderr, "SERVEUR: Commande invalide.\n");
                continue; 
            }

            printf("SERVEUR: Nouvelle commande reçue\n");

            pthread_mutex_lock(&mutex_file);
            file_attente[premier] = cmd;
            premier = (premier + 1) % TAILLE_FILE;
            nb_requetes++;
            pthread_mutex_unlock(&mutex_file);

            sem_post(&sem_requetes);
        }
    }

    return 0;
}