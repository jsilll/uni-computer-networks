CC=gcc
LD=gcc
CFLAGS=-g -Wall -std=gnu99
LDFLAGS=-lm


.PHONY: all clean run

all: client server


client: client/centralized_messaging/commands.o client/main.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o client/client client/centralized_messaging/commands.o client/main.o

client/centralized_messaging/commands.o: client/centralized_messaging/commands.c client/centralized_messaging/commands.h
	$(CC) $(CFLAGS) -o client/centralized_messaging/commands.o -c client/centralized_messaging/commands.c

client/main.o: client/main.c client/interface.h client/parsing.h
	$(CC) $(CFLAGS) -o client/main.o -c client/main.c


server: server/state/operations.o server/main.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o server/server server/state/operations.o server/main.o

server/state/operations.o: server/state/operations.c server/state/operations.h 
	$(CC) $(CFLAGS) -o server/state/operations.o -c server/state/operations.c

server/main.o: server/main.c server/connection.h server/command_handling.h server/parsing.h server/command_args_parsing.h
	$(CC) $(CFLAGS) -o server/main.o -c server/main.c


clean:
	@echo Cleaning...
	rm -f client/*.o client/centralized_messaging/*.o client/client
	rm -f server/*.o server/state/*.o server/server

count:
	find . -wholename '*/client/*.c' | xargs wc -l | grep total
	find . -wholename '*/client/*.h' | xargs wc -l | grep total
	find . -wholename '*/server/*.c' | xargs wc -l | grep total
	find . -wholename '*/server/*.h' | xargs wc -l | grep total