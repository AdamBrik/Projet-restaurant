#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/protocole.h"

#define TUBE_R_S "/tmp/tube_R_vers_S"
#define TUBE_S_R "/tmp/tube_S_vers_R"

int main() {
    Commande cmd;
    Reponse rep;

    printf("[SERVEUR] Cuisine ouverte ! En attente de commandes...\n");

    while(1) {
        int fd_r_s = open(TUBE_R_S, O_RDONLY);
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            printf("[SERVEUR] Commande reçue : Table %d demande %dx Plat n°%d\n", 
                   cmd.numero_table, cmd.quantite, cmd.id_plat);

            // Préparation de la réponse
            rep.statut = 1; // 1 = Succès
            
            
            if (cmd.id_plat == 1) {
                snprintf(rep.message, sizeof(rep.message), "Vos %d Pizzas arrivent !", cmd.quantite);
            } else if (cmd.id_plat == 2) {
                snprintf(rep.message, sizeof(rep.message), "Vos %d Pâtes arrivent !", cmd.quantite);
            } else {
                snprintf(rep.message, sizeof(rep.message), "Plat inconnu mais on vous prépare un truc bon !");
            }

           
            int fd_s_r = open(TUBE_S_R, O_WRONLY);
            write(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);
        }
    }
    return 0;
}