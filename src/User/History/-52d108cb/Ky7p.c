#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/protocole.h"

// Structure de la table de routage[cite: 2]
typedef struct {
    int id_message;
    int id_client;
    int code_serveur;
    int actif;
} EntreeRoutage;

int main(int argc, char *argv[]) {
    // Vérification des paramètres (config, max terminaux, cache)[cite: 2]
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <fichier_config> <max_terminaux> <taille_cache>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int taille_cache = atoi(argv[3]);
    EntreeRoutage table_routage[taille_cache];
    for (int i = 0; i < taille_cache; i++) {
        table_routage[i].actif = 0;
    }

    // Le routeur crée les tubes pour s'assurer qu'ils existent
    char *tube_c_r = "/tmp/tube_C_vers_R";
    char *tube_r_c = "/tmp/tube_R_vers_C";
    char *tube_r_s = "/tmp/tube_R_vers_S";
    char *tube_s_r = "/tmp/tube_S_vers_R";
    mkfifo(tube_c_r, 0666);
    mkfifo(tube_r_c, 0666);
    mkfifo(tube_r_s, 0666);
    mkfifo(tube_s_r, 0666);

    fprintf(stderr, "[ROUTEUR] Démarré. Cache:%d messages max.\n", taille_cache);

    Commande cmd;
    Reponse rep;
    int compteur = 0;

    while(1) {
        // 1. Lire la requête du client
        int fd_c_r = open(tube_c_r, O_RDONLY);
        int nb_lus = read(fd_c_r, &cmd, sizeof(Commande));
        close(fd_c_r);

        if (nb_lus > 0) {
            compteur++;
            
            // Ajouter à la table de routage[cite: 2]
            int index = -1;
            for (int i = 0; i < taille_cache; i++) {
                if (table_routage[i].actif == 0) {
                    table_routage[i].id_message = compteur;
                    table_routage[i].id_client = cmd.numero_table;
                    table_routage[i].actif = 1;
                    index = i;
                    break;
                }
            }

            fprintf(stderr, "[ROUTEUR] Transfert vers la cuisine...\n");
            
            // 2. Envoyer au serveur de données
            int fd_r_s = open(tube_r_s, O_WRONLY);
            write(fd_r_s, &cmd, sizeof(Commande));
            close(fd_r_s);

            // 3. Attendre la réponse du serveur de données
            int fd_s_r = open(tube_s_r, O_RDONLY);
            read(fd_s_r, &rep, sizeof(Reponse));
            close(fd_s_r);

            // Retirer de la table de routage[cite: 2]
            if (index != -1) table_routage[index].actif = 0;

            fprintf(stderr, "[ROUTEUR] Réponse reçue, renvoi au client...\n");

            // 4. Renvoyer au client
            int fd_r_c = open(tube_r_c, O_WRONLY);
            write(fd_r_c, &rep, sizeof(Reponse));
            close(fd_r_c);
        }
    }
    return 0;
}