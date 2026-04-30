#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tube_vers_routeur> <tube_vers_client>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *tube_c_r = argv[1];
    char *tube_r_c = argv[2];

    Commande cmd;
    Reponse rep;

    fprintf(stderr, "CLIENT : Application lancée. En attente du scan du QR Code...\n");

    while(1) {
        int code_serveur, code_lieux, code_menus;

        if (scanf("%4d%4d%4d|", &code_serveur, &code_lieux, &code_menus) == 3) {
            
            fprintf(stderr, "CLIENT: Serveur:%04d Lieux:%04d Menu:%04d\n", 
                    code_serveur, code_lieux, code_menus);

            // Adaptation de ta structure Commande (à ajuster selon ton protocole.h)
            cmd.numero_table = code_lieux; // Par exemple, on utilise le code lieux comme identifiant
            cmd.id_plat = code_menus;      // On utilise le code menu
            
            // Envoyer la structure au routeur via le tube passé en paramètre
            int fd_c_r = open(tube_c_r, O_WRONLY);
            write(fd_c_r, &cmd, sizeof(Commande));
            close(fd_c_r);

            // Attendre la structure de réponse via le tube passé en paramètre
            int fd_r_c = open(tube_r_c, O_RDONLY);
            read(fd_r_c, &rep, sizeof(Reponse));
            close(fd_r_c);

            // 4. Affichage du résultat sur StdOut[cite: 2]
            printf("RÉPONSE DU SERVEUR :\nStatut: %d | Message : %s\n", rep.statut, rep.message);
            
        } else {
            // Nettoyage de l'entrée standard en cas de mauvaise saisie
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            fprintf(stderr, "CLIENT Erreur : Format du QR Code invalide. Réessayez.\n");
        }
    }

    return 0;
}