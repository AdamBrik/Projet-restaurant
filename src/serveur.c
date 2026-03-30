#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      
#include <arpa/inet.h>   

#define PORT 8080

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

   
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erreur : Échec de la création de la socket");
        exit(EXIT_FAILURE);
    }

   
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    address.sin_port = htons(PORT);                   

    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur : Échec du bind");
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_fd, 3) < 0) {
        perror("Erreur : Échec de la mise en écoute");
        exit(EXIT_FAILURE);
    }

    
    printf(" LE SERVEUR EST OUVERT SUR 127.0.0.1:%d \n", PORT);
    printf(" En attente d'un client...\n");
   

    
    client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_socket < 0) {
        perror("Erreur : Échec de la connexion avec le client");
        exit(EXIT_FAILURE);
    }

    printf("\n[SUCCÈS] Un client vient de pousser la porte du restaurant !\n");

 
    close(client_socket);
    close(server_fd);
    printf("Fermeture du serveur.\n");

    return EXIT_SUCCESS;
}