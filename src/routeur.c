#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/protocole.h"

#define TUBE_C_R "/tmp/tube_C_vers_R"
#define TUBE_R_C "/tmp/tube_R_vers_C"
#define TUBE_R_S "/tmp/tube_R_vers_S"
#define TUBE_S_R "/tmp/tube_S_vers_R"

int main() {
    Commande cmd;
    Reponse rep;

    mkfifo(TUBE_C_R, 0666);
    mkfifo(TUBE_R_C, 0666);
    mkfifo(TUBE_R_S, 0666);
    mkfifo(TUBE_S_R, 0666);

    printf("[ROUTEUR] Service démarré. En attente des clients...\n");

    while(1) {
        // Recevoir Commande du Client
        int fd_c_r = open(TUBE_C_R, O_RDONLY);
        int nb_lus = read(fd_c_r, &cmd, sizeof(Commande));
        close(fd_c_r);

        if (nb_lus > 0) {
            printf("[ROUTEUR] Transfert commande de la Table %d vers la Cuisine...\n", cmd.numero_table);
            
            // Envoyer Commande à la Cuisine
            int fd_r_s = open(TUBE_R_S, O_WRONLY);
            write(fd_r_s, &cmd, sizeof(Commande));
            close(fd_r_s);

            // Recevoir Réponse de la Cuisine
            int fd_s_r = open(TUBE_S_R, O_RDONLY);
            read(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);

            // Envoyer Réponse au Client
            int fd_r_c = open(TUBE_R_C, O_WRONLY);
            write(fd_r_c, &rep, sizeof(Reponse));
            close(fd_r_c);
            
            printf("[ROUTEUR] Réponse servie à la Table %d.\n", cmd.numero_table);
        }
    }
    return 0;
}