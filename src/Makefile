CC      = gcc

CFLAGS  = -Wall -pthread -std=gnu99

OBJECT  = main.o utils.o scheduler.o

EXEC    = mysched

default : $(EXEC)

$(EXEC) : $(OBJECT) 
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECT)

main.o : main.c utils.h list.h scheduler.h
	$(CC) $(CFLAGS) -c main.c

utils.o : utils.c utils.h list.h
	$(CC) $(CFLAGS) -c utils.c scheduler.c

clean :
	$(RM) $(EXEC) *.o *.out
