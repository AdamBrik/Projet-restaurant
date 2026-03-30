#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define TUBE_R_S "/tmp/tube_R_vers_S"
#define TUBE_S_R "/tmp/tube_S_vers_R"

int main() {
    char buffer[1024];
    char reponse[1024] = "Commande acceptée";

    printf("SERVEUR: Allumé. En attente du routeur...\n");

    int fd_r_s = open(TUBE_R_S, O_RDONLY);
    read(fd_r_s, buffer, sizeof(buffer));
    printf("SERVEUR: Commande reçue : %s\n", buffer);
    close(fd_r_s);

    int fd_s_r = open(TUBE_S_R, O_WRONLY);
    write(fd_s_r, reponse, strlen(reponse) + 1);
    close(fd_s_r);

    printf("SERVEUR: Réponse envoyée. Je ferme.\n");

    return 0;
}