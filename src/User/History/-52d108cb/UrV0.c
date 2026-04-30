#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/protocole.h"

typedef struct {
    int id_message;
    int id_client;
    int code_serveur;
    int actif;
} EntreeRoutage;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <fichier_config> <max_terminaux> <taille_cache>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int taille_cache = atoi(argv[3]);
    EntreeRoutage table_routage[taille_cache];
    for (int i = 0; i < taille_cache; i++) {
        table_routage[i].actif = 0;
    }

    char *tube_c_r = "/tmp/tube_C_vers_R";
    char *tube_r_c = "/tmp/tube_R_vers_C";
    char *tube_r_s = "/tmp/tube_R_vers_S";
    char *tube_s_r = "/tmp/tube_S_vers_R";
    mkfifo(tube_c_r, 0666); mkfifo(tube_r_c, 0666);
    mkfifo(tube_r_s, 0666); mkfifo(tube_s_r, 0666);

    fprintf(stderr, "[ROUTEUR] Démarré. Cache:%d\n", taille_cache);
    Commande cmd; Reponse rep;
    int compteur = 0;

    while(1) {
        int fd_c_r = open(tube_c_r, O_RDONLY);
        int nb_lus = read(fd_c_r, &cmd, sizeof(Commande));
        close(fd_c_r);

        if (nb_lus > 0) {
            if (!commande_est_valide(&cmd)) {
                fprintf(stderr, "[ROUTEUR] Commande invalide ignorée.\n");
                continue;
            }

            compteur++;
            int index = -1;
            for (int i = 0; i < taille_cache; i++) {
                if (table_routage[i].actif == 0) {
                    table_routage[i].id_message = compteur;
                    table_routage[i].id_client = cmd.id_client;
                    table_routage[i].code_serveur = cmd.code_serveur;
                    table_routage[i].actif = 1;
                    index = i;
                    break;
                }
            }

            int fd_r_s = open(tube_r_s, O_WRONLY);
            write(fd_r_s, &cmd, sizeof(Commande));
            close(fd_r_s);

            int fd_s_r = open(tube_s_r, O_RDONLY);
            read(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);

            if (index != -1) table_routage[index].actif = 0;

            int fd_r_c = open(tube_r_c, O_WRONLY);
            write(fd_r_c, &rep, sizeof(Reponse));
            close(fd_r_c);
        }
    }
    return 0;
}