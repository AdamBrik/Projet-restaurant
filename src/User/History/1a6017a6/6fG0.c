#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    // 1. Les paramètres du main[cite: 2]
    // Le client a besoin du nom du tube vers le routeur et du tube vers le client[cite: 2].
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tube_vers_routeur> <tube_vers_client>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *tube_c_r = argv[1];
    char *tube_r_c = argv[2];

    Commande cmd;
    Reponse rep;

    // 2. Les logs sur StdErr[cite: 2]
    fprintf(stderr, "[CLIENT] Application lancée. En attente du scan du QR Code...\n");

    while(1) {
        int code_serveur, code_lieux, code_menus;

        // 3. Lecture du QR code sur StdIn avec le format exact demandé[cite: 2]
        // Format attendu : <Code serveur 4 chiffres><Code lieux 4 chiffres><Code menus 4 chiffres>|[cite: 2]
        if (scanf("%4d%4d%4d|", &code_serveur, &code_lieux, &code_menus) == 3) {
            
            fprintf(stderr, "[CLIENT] QR Code lu ! Serveur:%04d Lieux:%04d Menu:%04d\n", 
                    code_serveur, code_lieux, code_menus);

            // Préparation de la commande
            cmd.numero_table = code_lieux; // On utilise le code lieux comme identifiant de table
            cmd.id_plat = code_menus;      // On utilise le code menu comme identifiant de plat
            cmd.quantite = 1;              // Par défaut, on commande 1 unité
            
            // Envoyer la structure au routeur via le tube passé en paramètre[cite: 2]
            int fd_c_r = open(tube_c_r, O_WRONLY);
            if (fd_c_r != -1) {
                write(fd_c_r, &cmd, sizeof(Commande));
                close(fd_c_r);
            } else {
                perror("[CLIENT] Erreur lors de l'ouverture du tube vers le routeur");
                continue;
            }

            // Attendre la structure de réponse via le tube passé en paramètre[cite: 2]
            int fd_r_c = open(tube_r_c, O_RDONLY);
            if (fd_r_c != -1) {
                read(fd_r_c, &rep, sizeof(Reponse));
                close(fd_r_c);
                
                // 4. Affichage du résultat sur StdOut (écran pour l'utilisateur)[cite: 2]
                printf("=== RÉPONSE DU SERVEUR ===\nStatut: %d | Message : %s\n\n", rep.statut, rep.message);
            } else {
                perror("[CLIENT] Erreur lors de l'ouverture du tube depuis le routeur");
            }
            
        } else {
            // Nettoyage de l'entrée standard en cas de mauvaise saisie
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            fprintf(stderr, "[CLIENT] Erreur : Format du QR Code invalide. Réessayez (ex: 123456780001|).\n");
        }
    }

    return 0;
}