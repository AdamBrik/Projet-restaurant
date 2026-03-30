CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = src
BIN_DIR = bin


all: directories client serveur routeur


directories:
	@mkdir -p $(BIN_DIR)


client: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(SRC_DIR)/client.c


serveur: $(SRC_DIR)/serveur.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/serveur $(SRC_DIR)/serveur.c


routeur: $(SRC_DIR)/routeur.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/routeur $(SRC_DIR)/routeur.c


clean:
	rm -rf $(BIN_DIR)/*
	rm -f /tmp/tube_*

.PHONY: all clean directories client serveur routeur