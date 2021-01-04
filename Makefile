JSON_C_DIR = ./json-c
RM = rm -rf

CC = gcc
CFLAGS += -g3 -Wno-variadic-macros -Wno-unused-function -Wall -std=c89 -pedantic -I$(JSON_C_DIR)/ -I$(JSON_C_DIR)-build/
LDFLAGS += -L$(JSON_C_DIR)-build -ljson-c -lncurses

valgrind: build_valgrind.sh debug
	bash ./build_valgrind.sh

debug: json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o
	$(CC) $(CFLAGS) -odebug json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.c ${LDFLAGS} && make clear

game: game.o json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o
	$(CC) $(CFLAGS) -ogame_sample json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.o ${LDFLAGS} && make clear

menu-lib.o: data/libs/menu-lib.c
	$(CC) $(CFLAGS) -omenu-lib.o -c data/libs/menu-lib.c ${LDFLAGS}

json-lib.o: data/libs/json-lib.c
	$(CC) $(CFLAGS) -ojson-lib.o -c data/libs/json-lib.c -Werror -Wall -Wextra ${LDFLAGS}

window-lib.o: data/libs/window-lib.c
	$(CC) $(CFLAGS) -owindow-lib.o -c data/libs/window-lib.c ${LDFLAGS}

agility-lib.o: data/libs/agility-lib.c
	$(CC) $(CFLAGS) -oagility-lib.o -c data/libs/agility-lib.c ${LDFLAGS}

story-lib.o: data/libs/story-lib.c
	$(CC) $(CFLAGS) -ostory-lib.o -c data/libs/story-lib.c ${LDFLAGS}

game-lib.o: data/libs/game-lib.c
	$(CC) $(CFLAGS) -ogame-lib.o -c data/libs/game-lib.c ${LDFLAGS}

game.o: game.c data/libs/lib.h
	$(CC) $(CFLAGS) -ogame.o -c game.c ${LDFLAGS}

lib: build_libs.sh
	bash ./build_libs.sh

clear:
	${RM} *.o

vclear:
	${RM} ./output/*

clean:
	${RM} *.o
	${RM} ./json-c
	${RM} ./json-c-build
	${RM} game_sample*
	${RM} debug

restart: 
	make clean && make game
