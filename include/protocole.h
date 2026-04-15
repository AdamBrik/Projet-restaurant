#ifndef PROTOCOLE_H
#define PROTOCOLE_H


typedef struct {
    int numero_table;
    int id_plat;       
    int quantite;
} Commande;


typedef struct {
    int statut;        
    char message[256]; 
} Reponse;

#endif