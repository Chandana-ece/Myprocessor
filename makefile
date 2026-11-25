
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJ = main.o file_inclusion.o comment_removal.o macro_handler.o

mypreprocessor: $(OBJ)
	gcc $(CFLAGS) -o mypreprocessor $(OBJ)

main.o: main.c header.h
	gcc $(CFLAGS) -c main.c

file_inclusion.o: file_inclusion.c header.h
	gcc $(CFLAGS) -c file_inclusion.c

comment_removal.o: comment_removal.c header.h
	gcc $(CFLAGS) -c comment_removal.c

macro_handler.o: macro_handler.c header.h
	gcc $(CFLAGS) -c macro_handler.c

clean:
	rm -f *.o mypreprocessor

