#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/protocole.h"

int main(int argc, char *argv[]) {
    // 1. Récupération des paramètres de lancement[cite: 2]
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

    // Utilisation de StdErr pour les logs de fonctionnement[cite: 2]
    fprintf(stderr, "[SERVEUR] Cuisine ouverte ! Fichier de configuration de base : %s\n", fichier_config);

    while(1) {
        // 2. Le serveur lit en boucle sur le tube entrant[cite: 2]
        int fd_r_s = open(tube_r_s, O_RDONLY);
        if (fd_r_s == -1) {
            perror("[SERVEUR] Erreur lors de l'ouverture du tube entrant");
            continue; // On passe à la prochaine itération au lieu de planter
        }
        
        int nb_lus = read(fd_r_s, &cmd, sizeof(Commande)); 
        close(fd_r_s);

        if (nb_lus > 0) {
            fprintf(stderr, "[SERVEUR] Commande reçue : Lieu %d, Menu %d\n", cmd.numero_table, cmd.id_plat);

            // 3. Le fichier de menu correspondant est chargé[cite: 2]
            char nom_fichier[50];
            // On fabrique le nom du fichier dynamiquement (ex: "menu_1.txt", "menu_2.txt")
            snprintf(nom_fichier, sizeof(nom_fichier), "menu_%d.txt", cmd.id_plat);

            // Ouverture du fichier en mode lecture ("r" pour read)
            FILE *fichier = fopen(nom_fichier, "r");

            if (fichier != NULL) {
                // Le fichier existe ! On prépare la lecture
                char buffer_lecture[256];
                rep.message[0] = '\0'; // On s'assure que le message est vide au départ
                rep.statut = 1; // 1 = Succès
                
                // On lit la première ligne du fichier (KISS : on reste simple pour le MVP)[cite: 2]
                if (fgets(buffer_lecture, sizeof(buffer_lecture), fichier) != NULL) {
                    // Nettoyage du saut de ligne potentiel à la fin du fgets
                    buffer_lecture[strcspn(buffer_lecture, "\n")] = 0;
                    
                    // On copie le texte lu dans la réponse à envoyer au routeur
                    snprintf(rep.message, sizeof(rep.message), "Voici le menu : %s", buffer_lecture);
                } else {
                    snprintf(rep.message, sizeof(rep.message), "Le fichier %s est vide.", nom_fichier);
                }
                
                // Fermeture propre du fichier une fois la lecture terminée
                fclose(fichier);

            } else {
                // Le fichier n'existe pas (fopen a renvoyé NULL)
                snprintf(rep.message, sizeof(rep.message), "Désolé, le menu %d n'est pas disponible.", cmd.id_plat);
                rep.statut = 0; // 0 = Échec
            }

            // 4. Le message de réponse est envoyé au serveur de routage[cite: 2]
            int fd_s_r = open(tube_s_r, O_WRONLY);
            if (fd_s_r != -1) {
                write(fd_s_r, &rep, sizeof(Reponse));
                close(fd_s_r);
                fprintf(stderr, "[SERVEUR] Réponse envoyée au routeur.\n");
            } else {
                perror("[SERVEUR] Erreur d'ouverture du tube sortant");
            }
        }
    }
    return 0;
}