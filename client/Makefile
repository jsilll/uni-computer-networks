OBJS	= main.o
SOURCE	= src/main.c
HEADER	= src/centralized_messaging_api.h
OUT  	= user
CC	    = gcc
FLAGS	= -g -c -Wall
LFLAGS	= 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

# create/compile the individual files >>separately<<
main.o: src/main.c
	$(CC) $(FLAGS) src/main.c 

# clean house
clean:
	rm -f $(OBJS) $(OUT)