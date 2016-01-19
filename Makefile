CC=gcc
LD=gcc

RM=rm
MKDIR=mkdir

SRC_DIR=src
BUILD_DIR=build
BIN_DIR=$(BUILD_DIR)/bin
OBJ_DIR=$(BUILD_DIR)/obj

CFLAGS=--debug
LDFLAGS=-lpthread --debug

all: .pre-build $(BIN_DIR)/main

.pre-build: $(BUILD_DIR).dir $(BIN_DIR).dir $(OBJ_DIR).dir

%.dir:
	$(MKDIR) -p $(subst .dir,,$@)

$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) -rf $(BUILD_DIR)
