#include <stdio.h>
#include <stdlib.h>
#include "protocole.h"

int main() {
    printf("==========================================\n");
    printf("   BIENVENUE DANS LE RESTAURANT DE L'OS   \n");
    printf("==========================================\n");
    printf("Le client a démarré avec succès !\n");
    printf("Taille max d'un message définie à : %d octets\n", TAILLE_MAX_MESSAGE);
    
    return EXIT_SUCCESS;
}