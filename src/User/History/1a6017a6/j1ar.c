#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    // Vérification des paramètres (tubes entrants/sortants)[cite: 2]
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tube_vers_routeur> <tube_vers_client>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *tube_c_r = argv[1];
    char *tube_r_c = argv[2];

    Commande cmd;
    Reponse rep;

    // Logs sur la sortie d'erreur standard[cite: 2]
    fprintf(stderr, "[CLIENT] Application lancée. En attente du scan du QR Code...\n");

    while(1) {
        int code_serveur, code_lieux, code_menus;

        // Lecture du QR code sur StdIn avec le format imposé[cite: 2]
        if (scanf("%4d%4d%4d|", &code_serveur, &code_lieux, &code_menus) == 3) {
            
            fprintf(stderr, "[CLIENT] QR Code lu ! Serveur:%04d Lieux:%04d Menu:%04d\n", code_serveur, code_lieux, code_menus);

            // Préparation de la commande
            cmd.numero_table = code_lieux; 
            cmd.id_plat = code_menus;      
            cmd.quantite = 1;              
            
            // Envoi au routeur
            int fd_c_r = open(tube_c_r, O_WRONLY);
            write(fd_c_r, &cmd, sizeof(Commande));
            close(fd_c_r);

            // Attente de la réponse
            int fd_r_c = open(tube_r_c, O_RDONLY);
            read(fd_r_c, &rep, sizeof(Reponse));
            close(fd_r_c);
            
            // Affichage du menu
            printf("\n=== MENU DU RESTAURANT ===\n%s\n", rep.message);

            // Choix interactif du plat (fonctionnalité locale)
            if (rep.statut == 1) { 
                int choix_plat, quantite_plat;
                printf("Que voulez-vous commander (entrez le numéro du plat) ? > ");
                if (scanf("%d", &choix_plat) == 1) {
                    printf("Combien en voulez-vous ? > ");
                    if (scanf("%d", &quantite_plat) == 1) {
                        printf("\n[CLIENT] Succès ! Commande validée : %d x Plat n°%d.\n", quantite_plat, choix_plat);
                        printf("--------------------------------------------------\n\n");
                    }
                }
            } else {
                printf("--------------------------------------------------\n\n");
            }
            
        } else {
            // Nettoyage si erreur de frappe
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            fprintf(stderr, "[CLIENT] Erreur : Format invalide. Réessayez (ex: 123456780001|).\n");
        }
    }
    return 0;
}