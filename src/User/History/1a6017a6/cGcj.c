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
    Commande cmd; Reponse rep;
    int mon_id = getpid(); 

    fprintf(stderr, "[CLIENT %d] Lancé. En attente du QR Code...\n", mon_id);

    while(1) {
        int code_serveur, code_lieux, code_menus;
        if (scanf("%4d%4d%4d|", &code_serveur, &code_lieux, &code_menus) == 3) {
            
            cmd.code_serveur = code_serveur;
            cmd.numero_table = code_lieux; 
            cmd.id_plat = code_menus;      
            cmd.quantite = 1;
            cmd.id_client = mon_id; 
            
            if (!commande_est_valide(&cmd)) {
                fprintf(stderr, "[CLIENT] QR Code contient des données corrompues.\n");
                continue;
            }

            int fd_c_r = open(tube_c_r, O_WRONLY);
            if (fd_c_r != -1) {
                write(fd_c_r, &cmd, sizeof(Commande));
                close(fd_c_r);
            }

            int fd_r_c = open(tube_r_c, O_RDONLY);
            if (fd_r_c != -1) {
                read(fd_r_c, &rep, sizeof(Reponse));
                close(fd_r_c);
                printf("\n=== MENU ===\n%s\n\n", rep.message);
            }
            
        } else {
            int c; while ((c = getchar()) != '\n' && c != EOF) { }
            fprintf(stderr, "[CLIENT] Format invalide.\n");
        }
    }
    return 0;
}