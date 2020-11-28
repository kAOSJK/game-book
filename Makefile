CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -Werror -Wextra

game_sample: game.o json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o
	$(CC) -ogame_sample json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game.o -lncurses -ljson-c -static -static-libgcc

menu-lib.o: data/libs/menu-lib.c
	$(CC) -omenu-lib.o -c data/libs/menu-lib.c $(CFLAGS) -lncurses

json-lib.o: data/libs/json-lib.c
	$(CC) -ojson-lib.o -c data/libs/json-lib.c -Werror -Wall -Wextra -ljson-c

window-lib.o: data/libs/window-lib.c
	$(CC) -owindow-lib.o -c data/libs/window-lib.c $(CFLAGS) -lncurses

agility-lib.o: data/libs/agility-lib.c
	$(CC) -oagility-lib.o -c data/libs/agility-lib.c $(CFLAGS) -lncurses

story-lib.o: data/libs/story-lib.c
	$(CC) -ostory-lib.o -c data/libs/story-lib.c -lncurses

game.o: game.c data/libs/lib.h
	$(CC) -ogame.o -c game.c -lncurses

clear:
	rm *.o

clean:
	rm *.o game_sample.exe
