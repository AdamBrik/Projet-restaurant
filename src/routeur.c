#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define TUBE_C_R "/tmp/tube_C_vers_R"
#define TUBE_R_C "/tmp/tube_R_vers_C"
#define TUBE_R_S "/tmp/tube_R_vers_S"
#define TUBE_S_R "/tmp/tube_S_vers_R"

int main() {
    char buffer[1024];

    mkfifo(TUBE_C_R, 0666);
    mkfifo(TUBE_R_C, 0666);
    mkfifo(TUBE_R_S, 0666);
    mkfifo(TUBE_S_R, 0666);

    printf("ROUTEUR: En attente du client...\n");

    int fd_c_r = open(TUBE_C_R, O_RDONLY);
    read(fd_c_r, buffer, sizeof(buffer));
    printf("ROUTEUR: Message reçu du client : %s\n", buffer);
    close(fd_c_r);

    printf("ROUTEUR: Transfert vers le serveur...\n");
    int fd_r_s = open(TUBE_R_S, O_WRONLY);
    write(fd_r_s, buffer, strlen(buffer) + 1);
    close(fd_r_s);

    int fd_s_r = open(TUBE_S_R, O_RDONLY);
    read(fd_s_r, buffer, sizeof(buffer));
    printf("ROUTEUR: Réponse reçue du serveur : %s\n", buffer);
    close(fd_s_r);

    printf("ROUTEUR: Renvoi au client...\n");
    int fd_r_c = open(TUBE_R_C, O_WRONLY);
    write(fd_r_c, buffer, strlen(buffer) + 1);
    close(fd_r_c);

    printf("ROUTEUR: Terminé.\n");

    unlink(TUBE_C_R); unlink(TUBE_R_C); unlink(TUBE_R_S); unlink(TUBE_S_R);

    return 0;
}