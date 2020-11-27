CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -Werror -Wextra

game_sample: game.o json-lib.o window-lib.o menu-lib.o agility-lib.o
	$(CC) -ogame_sample json-lib.o window-lib.o menu-lib.o agility-lib.o game.o -lncurses -ljson-c -static -static-libgcc

menu-lib.o: menu-lib.c
	$(CC) -omenu-lib.o -c menu-lib.c $(CFLAGS) -lncurses

json-lib.o: json-lib.c
	$(CC) -ojson-lib.o -c json-lib.c -Werror -Wall -Wextra -ljson-c

window-lib.o: window-lib.c
	$(CC) -owindow-lib.o -c window-lib.c $(CFLAGS) -lncurses

agility-lib.o: agility-lib.c
	$(CC) -oagility-lib.o -c agility-lib.c $(CFLAGS) -lncurses

game.o: game.c lib.h
	$(CC) -ogame.o -c game.c -lncurses

clean:
	rm *.o game_sample.exe
