# Makefile

CC = gcc
CPPFLGS = -MMD
CFLAGS = -Wall -Wextra -fsanitize=address
LDFLAGS = -fsanitize=address
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf`

SRC = main.c draw_result.c file.c
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
