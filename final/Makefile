# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = `pkg-config --cflags gtk+-3.0`  -Wall -Wextra -std=c99 -O3 -fsanitize=address
LDFLAGS = `pkg-config --libs gtk+-3.0` -lSDL2 -lSDL2_image -lm -fsanitize=address

SRC = main.c src/network/network.c src/network/ocr.c src/network/training.c src/utils/utils.c src/ui/ui.c src/ui/train.c src/ui/resolve.c
OBJ = ${SRC:.c=.o}
DEP = ${OBJ:.o=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} main

# END
