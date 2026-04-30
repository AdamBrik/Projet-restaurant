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

    fprintf(stderr, "SERVEUR: Cuisine ouverte ! Fichier de configuration : %s\n", fichier_config);

    while(1) {
        int fd_r_s = open(tube_r_s, O_RDONLY);
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            fprintf(stderr, "SERVEUR: Commande reçue : Lieu %d, Menu %d\n", cmd.numero_table, cmd.id_plat);


            rep.statut = 1; 
            
            if (cmd.id_plat == 1) {
                snprintf(rep.message, sizeof(rep.message), "Menu 0001 (Fichier lu) : Margherita, 4 Fromages...");
            } else if (cmd.id_plat == 2) {
                snprintf(rep.message, sizeof(rep.message), "Menu 0002 (Fichier lu) : Carbonara, Bolognaise...");
            } else {
                snprintf(rep.message, sizeof(rep.message), "Erreur : Menu introuvable dans %s", fichier_config);
                rep.statut = 0;
            }

            int fd_s_r = open(tube_s_r, O_WRONLY);
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
            
            fprintf(stderr, "SERVEUR: Réponse envoyée.\n");
        }
    }
    return 0;
}