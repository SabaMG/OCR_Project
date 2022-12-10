# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99 -O0 -g -fsanitize=address
LDFLAGS = -lm -lSDL2 -lSDL2_image -fsanitize=address
#CFLAGS = -Wall -Wextra -std=c99 -O0
#LDFLAGS = -lm -lSDL2 -lSDL2_image

SRC = main.c ocr.c training.c network.c
OBJ = ${SRC:.c=.o}
DEP = ${OBJ:.o=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} main

-include ${DEP}

# END
