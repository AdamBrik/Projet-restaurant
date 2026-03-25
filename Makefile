# --- Variables ---
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGETS = $(BIN_DIR)/client

all: directories $(TARGETS)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(BIN_DIR)/client: $(OBJ_DIR)/client.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*

.PHONY: all directories clean