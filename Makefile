CC = gcc
#CFLAGS = -std=c89 -pedantic -Wall -Werror -Wextra
RM = rm -rf

JSON_C_DIR = ./json-c

CFLAGS += -g3 -I$(JSON_C_DIR)/ -I$(JSON_C_DIR)-build/
LDFLAGS += -L$(JSON_C_DIR)-build -ljson-c -lncurses
LD_LIBRARY_PATH=./json-c-build/:$LD_LIBRARY_PATH #in case of it doens't find the freshly created lib

DEBUG = --tool=memcheck --leak-check=yes --show-reachable=yes --track-origins=yes --log-file="./output/valgrind-$(date +"%d-%m-%Y")"

# DEBUGGING CASE - ONLY WORKING ON LINUX SYSTEM
valgrind: debug
	bash ./build_valgrind.sh

debug: json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o
	$(CC) $(CFLAGS) -odebug json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.c ${LDFLAGS} && make clear

lib:
	bash ./build_libs.sh

# GAME COMPILITING CASE - ONLY WORKING ON WINDOWS
game: game.o json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o
	$(CC) $(CFLAGS) -ogame_sample json-lib.o window-lib.o menu-lib.o agility-lib.o story-lib.o game-lib.o game.o -lncurses -ljson-c -static -static-libgcc && make clear

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

clear:
	${RM} *.o

cle:
	${RM} ./output/*

clean:
	${RM} *.o
	${RM} debug

restart: 
	make clean && make game
