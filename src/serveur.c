#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define NB_CLIENTS 2
#define IP INADDR_ANY
#define PORT 8080
#define BACKLOG 3

int main() {
    int clients[NB_CLIENTS]; 
    char buffer[1024] = {0}; 

    struct sockaddr_in adresse;
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = IP;     
    adresse.sin_port = htons(PORT);   

    int fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fdsocket == -1) {
        perror("Erreur socket");
        return 0;
    }

    int result = bind(fdsocket, (struct sockaddr *)&adresse, sizeof(adresse));
    if (result != 0) {
        perror("Erreur bind ");
        return 0;
    }


    result = listen(fdsocket, BACKLOG);
    if (result != 0) {
        perror("Erreur listen");
        return 0;
    }

    
    printf(" LE SERVEUR EST OUVERT SUR LE PORT %d \n", PORT);
    

    struct sockaddr_in client_adresse;
    int taille = sizeof(client_adresse);

   
    int client = accept(fdsocket, (struct sockaddr *)&client_adresse, (socklen_t *)&taille);
    if (client == -1) {
        perror("Erreur client");
        return 0;
    }

    printf("SUCCÈS : Un client vient de se connecter\n");

  
    read(client, buffer, sizeof(buffer));
    printf("Message reçu du client : '%s'\n", buffer);

  
    close(client);
    close(fdsocket);
    printf("Fermeture du restaurant.\n");

    return 0;
}