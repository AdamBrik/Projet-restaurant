#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/protocole.h"

#define TUBE_C_R "/tmp/tube_C_vers_R"
#define TUBE_R_C "/tmp/tube_R_vers_C"

int main() {
    Commande cmd;
    Reponse rep;

    printf("=== BORNE DE COMMANDE ===\n");
    
    cmd.numero_table = 4;

    while(1) {
        printf("\nMenu : 1=Pizza, 2=Pâtes, 0=Quitter\n");
        printf("Que voulez-vous commander (entrez l'ID du plat) ? > ");
        scanf("%d", &cmd.id_plat);

        if (cmd.id_plat == 0) {
            printf("Au revoir !\n");
            break; 
        }

        printf("Combien en voulez-vous ? > ");
        scanf("%d", &cmd.quantite);

        // 1. Envoyer la structure au routeur
        int fd_c_r = open(TUBE_C_R, O_WRONLY);
        write(fd_c_r, &cmd, sizeof(Commande));
        close(fd_c_r);

        // 2. Attendre la structure de réponse
        int fd_r_c = open(TUBE_R_C, O_RDONLY);
        read(fd_r_c, &rep, sizeof(Reponse));
        close(fd_r_c);

        printf("[RÉPONSE] Statut: %d | Message : %s\n", rep.statut, rep.message);
    }

    return 0;
}