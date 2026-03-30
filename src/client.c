#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define TUBE_C_R "/tmp/tube_C_vers_R"
#define TUBE_R_C "/tmp/tube_R_vers_C"

int main() {
    char message[] = "Bonjour, je voudrais une table !";
    char buffer[1024];

    printf("[CLIENT] Connexion au routeur...\n");

    int fd_c_r = open(TUBE_C_R, O_WRONLY);
    write(fd_c_r, message, strlen(message) + 1);
    close(fd_c_r);
    printf("[CLIENT] Message envoyé : %s\n", message);

    int fd_r_c = open(TUBE_R_C, O_RDONLY);
    read(fd_r_c, buffer, sizeof(buffer));
    close(fd_r_c);

    printf("[CLIENT] Réponse finale reçue : %s\n", buffer);

    return 0;
}