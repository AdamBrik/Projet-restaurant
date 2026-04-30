#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <tube_entrant> <tube_sortant> <fichier_lieux_menus>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *tube_r_s = argv[1];
    char *tube_s_r = argv[2];
    char *fichier_config = argv[3];

    Commande cmd;
    Reponse rep;

    fprintf(stderr, "[SERVEUR] Cuisine ouverte ! Config : %s\n", fichier_config);

    while(1) {
        int fd_r_s = open(tube_r_s, O_RDONLY);
        if (fd_r_s == -1) continue;
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            if (!commande_est_valide(&cmd)) {
                fprintf(stderr, "[SERVEUR] Commande invalide rejetée.\n");
                continue; 
            }

            fprintf(stderr, "[SERVEUR] Commande reçue : Lieu %d, Menu %d\n", cmd.numero_table, cmd.id_plat);
            rep.id_client = cmd.id_client; 
            rep.statut = 0; 
            
            FILE *config = fopen(fichier_config, "r");
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
                    char buffer_lecture[180]; 
                    if (fgets(buffer_lecture, sizeof(buffer_lecture), fichier_menu) != NULL) {
                        buffer_lecture[strcspn(buffer_lecture, "\n")] = 0;
                        snprintf(rep.message, sizeof(rep.message), "[%s] Voici le menu : %s", nom_resto, buffer_lecture);
                        rep.statut = 1;
                    } else {
                        snprintf(rep.message, sizeof(rep.message), "Le menu est vide.");
                    }
                    fclose(fichier_menu);
                } else {
                    snprintf(rep.message, sizeof(rep.message), "[%s] Erreur: Le fichier %s est introuvable.", nom_resto, nom_fichier_cible);
                }
            } else {
                snprintf(rep.message, sizeof(rep.message), "Désolé, le lieu %d n'existe pas.", cmd.numero_table);
            }

            int fd_s_r = open(tube_s_r, O_WRONLY);
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
        }
    }
    return 0;
}