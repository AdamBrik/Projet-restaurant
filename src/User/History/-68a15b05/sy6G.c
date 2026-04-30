#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    // 1. Les paramètres du main
    // Le serveur a besoin du tube entrant, du tube sortant, et du fichier contenant les lieux/menus[cite: 2].
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <tube_entrant> <tube_sortant> <fichier_lieux_menus>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *tube_r_s = argv[1];
    char *tube_s_r = argv[2];
    char *fichier_config = argv[3];

    Commande cmd;
    Reponse rep;

    // Logs sur StdErr
    fprintf(stderr, "[SERVEUR] Cuisine ouverte ! Fichier de configuration : %s\n", fichier_config);

    while(1) {
        // 2. Le serveur lit en boucle sur le tube entrant[cite: 2]
        int fd_r_s = open(tube_r_s, O_RDONLY);
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            fprintf(stderr, "[SERVEUR] Commande reçue : Lieu %d, Menu %d\n", cmd.numero_table, cmd.id_plat);

            // 3. Le fichier de menu correspondant est chargé[cite: 2]
            // Dans ce MVP, on va faire semblant de lire le fichier pour ne pas surcomplexifier, 
            // mais on utilise l'ID du menu demandé pour formuler la réponse.
            rep.statut = 1; 
            
            // Plus tard, tu remplaceras ce if/else par une vraie ouverture de fichier texte (fopen, fgets...)
            if (cmd.id_plat == 1) {
                snprintf(rep.message, sizeof(rep.message), "Menu 0001 (Fichier lu) : Margherita, 4 Fromages...");
            } else if (cmd.id_plat == 2) {
                snprintf(rep.message, sizeof(rep.message), "Menu 0002 (Fichier lu) : Carbonara, Bolognaise...");
            } else {
                snprintf(rep.message, sizeof(rep.message), "Erreur : Menu introuvable dans %s", fichier_config);
                rep.statut = 0;
            }

            // 4. Le message de réponse est envoyé au serveur de routage[cite: 2]
            int fd_s_r = open(tube_s_r, O_WRONLY);
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
            
            fprintf(stderr, "[SERVEUR] Réponse envoyée.\n");
        }
    }
    return 0;
}