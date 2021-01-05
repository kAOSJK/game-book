JSON_C_DIR = ./json-c
RM = rm -rf

CC = gcc
CFLAGS = -Wno-variadic-macros -Wno-unused-function -Wall -std=c89 -pedantic
CFLAGS += -g3 -I$(JSON_C_DIR)/ -I$(JSON_C_DIR)-build/
LDFLAGS += -L$(JSON_C_DIR)-build -ljson-c -lncurses

valgrind: build_valgrind.sh debug
	bash ./build_valgrind.sh

debug: json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.o
	$(CC) $(CFLAGS) -o$@ $^ ${LDFLAGS} && make clear

ubuntu-game: json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.o
	$(CC) $(CFLAGS) -o$@_sample $^ ${LDFLAGS} && make clear

game: json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.o
	$(CC) $(CFLAGS) -o$@_sample $^ ${LDFLAGS} -static -lpthread -static-libgcc && make clear

menu-lib.o: data/libs/menu-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

json-lib.o: data/libs/json-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

window-lib.o: data/libs/window-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

agility-lib.o: data/libs/agility-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

story-lib.o: data/libs/story-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

game-lib.o: data/libs/game-lib.c
	$(CC) $(CFLAGS) -o$@ -c $^ ${LDFLAGS}

game.o: game.c data/libs/lib.h
	$(CC) $(CFLAGS) -o$@ -c game.c ${LDFLAGS}

lib: build_libs.sh
	bash ./$^

clear:
	${RM} *.o

vclear:
	${RM} ./output/*

clean: clear
	${RM} game_sample*
	${RM} debug
	${RM} ./json-c
	${RM} ./json-c-build

restart: 
	make clean && make game
