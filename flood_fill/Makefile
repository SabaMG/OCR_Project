# Makefile

CC = gcc -g
CPPFLGS = -MMD
CFLAGS = -Wall -Wextra -fsanitize=address -g
LDFLAGS = -fsanitize=address -g
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf`

SRC = main.c flood_fill.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

-include : ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
