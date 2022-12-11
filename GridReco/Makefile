# Makefile

CC = gcc
CPPFLGS = -MMD
CFLAGS = -Wall -Wextra -fsanitize=address -g
LDFLAGS = -fsanitize=address -lm
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

SRC = main.c tools.c center_number.c #../warp/warp.c
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
