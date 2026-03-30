#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define NB_CLIENTS 2
#define IP "127.0.0.1"
#define PORT 8080

int main() {
    struct sockaddr_in adresse;
    char *message = "Bonjour Serveur, je voudrais reserver une table !";

    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = inet_addr(IP); 
    adresse.sin_port = htons(PORT);         

    
    int fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fdsocket == -1) {
        perror("Erreur lors de la création de la socket");
        return 0;
    }

    printf("Tentative de connexion au serveur %s:%d...\n", IP, PORT);

   
    int result = connect(fdsocket, (struct sockaddr *)&adresse, sizeof(adresse));
    if (result != 0) {
        perror("Erreur connect (Le serveur est-il bien allumé ?)");
        return 0;
    }

    printf("[SUCCÈS] Connecté au serveur !\n");

    
    send(fdsocket, message, strlen(message), 0);
    printf("Message envoyé : '%s'\n", message);

  
    close(fdsocket);
    
    return 0;
}