CC=gcc
LD=gcc
CFLAGS=-g -Wall -std=gnu99
LDFLAGS=-lm

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

CLIENT_DIR=client
CLIENT_BIN=client
CLIENT_SRC_DIR=$(CLIENT_DIR)/$(SRC_DIR)
CLIENT_OBJ_DIR=$(CLIENT_DIR)/$(OBJ_DIR)
CLIENT_BIN_DIR=$(CLIENT_DIR)/$(BIN_DIR)
CLIENT_SRCS=$(wildcard $(CLIENT_SRC_DIR)/*.c)
CLIENT_OBJS=$(patsubst $(CLIENT_SRC_DIR)/%.c, $(CLIENT_OBJ_DIR)/%.o,\
$(CLIENT_SRCS))

SERVER_DIR=server
SERVER_BIN=server
SERVER_SRC_DIR=$(SERVER_DIR)/$(SRC_DIR)
SERVER_OBJ_DIR=$(SERVER_DIR)/$(OBJ_DIR)
SERVER_BIN_DIR=$(SERVER_DIR)/$(BIN_DIR)
SERVER_SRCS=$(wildcard $(SERVER_SRC_DIR)/*.c)
SERVER_OBJS=$(patsubst $(SERVER_SRC_DIR)/%.c, $(SERVER_OBJ_DIR)/%.o,\
$(SERVER_SRCS))

.PHONY: all clean 

all: $(CLIENT_BIN_DIR)/$(CLIENT_BIN) $(SERVER_BIN_DIR)/$(SERVER_BIN)

# Client
$(CLIENT_BIN_DIR)/$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Server
$(SERVER_BIN_DIR)/$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	@echo Cleaning...
	$(RM) -r $(CLIENT_BIN_DIR)/* $(CLIENT_OBJ_DIR)/*
	$(RM) -r $(SERVER_BIN_DIR)/* $(SERVER_OBJ_DIR)/*

# count:
# 	find . -wholename '*/client/*.c' | xargs wc -l | grep total
# 	find . -wholename '*/client/*.h' | xargs wc -l | grep total
# 	find . -wholename '*/server/*.c' | xargs wc -l | grep total
# 	find . -wholename '*/server/*.h' | xargs wc -l | grep total