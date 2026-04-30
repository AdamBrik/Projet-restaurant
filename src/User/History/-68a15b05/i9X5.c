#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    // Vérification des paramètres (tubes entrants/sortants, fichier config)[cite: 2]
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
        // Lire la demande du routeur
        int fd_r_s = open(tube_r_s, O_RDONLY);
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            fprintf(stderr, "[SERVEUR] Commande reçue : Lieu %d, Menu %d\n", cmd.numero_table, cmd.id_plat);

            // Charger dynamiquement le fichier de menu[cite: 2]
            char nom_fichier[50];
            snprintf(nom_fichier, sizeof(nom_fichier), "menu_%d.txt", cmd.id_plat);

            FILE *fichier = fopen(nom_fichier, "r");

            if (fichier != NULL) {
                char buffer_lecture[200]; // Limité à 200 pour éviter le warning de troncature
                rep.statut = 1; 
                
                if (fgets(buffer_lecture, sizeof(buffer_lecture), fichier) != NULL) {
                    buffer_lecture[strcspn(buffer_lecture, "\n")] = 0; // Enlever le saut de ligne
                    snprintf(rep.message, sizeof(rep.message), "Voici le menu : %s", buffer_lecture);
                } else {
                    snprintf(rep.message, sizeof(rep.message), "Le fichier %s est vide.", nom_fichier);
                }
                fclose(fichier);
            } else {
                snprintf(rep.message, sizeof(rep.message), "Désolé, le menu %d n'est pas disponible.", cmd.id_plat);
                rep.statut = 0; 
            }

            // Renvoyer la réponse au routeur
            int fd_s_r = open(tube_s_r, O_WRONLY);
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
            
            fprintf(stderr, "[SERVEUR] Réponse envoyée au routeur.\n");
        }
    }
    return 0;
}