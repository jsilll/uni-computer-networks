CC=gcc
CFLAGS=-g -Wall

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

CLIENT_DIR=client
CLIENT_BIN=User
CLIENT_SRC_DIR=$(CLIENT_DIR)/$(SRC_DIR)
CLIENT_OBJ_DIR=$(CLIENT_DIR)/$(OBJ_DIR)
CLIENT_BIN_DIR=$(CLIENT_DIR)/$(BIN_DIR)
CLIENT_SRCS=$(wildcard $(CLIENT_SRC_DIR)/*.c)
CLIENT_OBJS=$(patsubst $(CLIENT_SRC_DIR)/%.c, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SRCS))

SERVER_DIR=server
SERVER_BIN=DS
SERVER_SRC_DIR=$(SERVER_DIR)/$(SRC_DIR)
SERVER_OBJ_DIR=$(SERVER_DIR)/$(OBJ_DIR)
SERVER_BIN_DIR=$(SERVER_DIR)/$(BIN_DIR)
SERVER_SRCS=$(wildcard $(SERVER_SRC_DIR)/*.c)
SERVER_OBJS=$(patsubst $(SERVER_SRC_DIR)/%.c, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SRCS))

SUBMISSION=submission.zip

.PHONY: all submission clean 

all: $(CLIENT_BIN) $(SERVER_BIN)

submission: clean
	zip -r $(SUBMISSION) $(CLIENT_DIR) $(SERVER_DIR) README.md Makefile 

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	@echo Cleaning...
	@$(RM) -r $(CLIENT_BIN) ./$(CLIENT_OBJ_DIR)/*
	@$(RM) -r $(SERVER_BIN) ./$(SERVER_OBJ_DIR)/*
	@$(RM) $(SUBMISSION)