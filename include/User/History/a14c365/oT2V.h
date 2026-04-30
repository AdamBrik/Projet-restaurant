#ifndef PROTOCOLE_H
#define PROTOCOLE_H

typedef struct {
    int code_serveur; 
    int numero_table; 
    int id_plat;      
    int quantite;
    int id_client;    
} Commande;

typedef struct {
    int statut;
    char message[256];
    int id_client;    
} Reponse;


int commande_est_valide(Commande *cmd) {
    if (cmd->code_serveur <= 0 || cmd->numero_table <= 0 || cmd->id_plat <= 0 || cmd->quantite <= 0) {
        return 0; 
    }
    return 1; 
}

#endif