# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99 -O0 -g
LDFLAGS = -lm

SRC = main.c neural_network.c utils.c impl.c file.c
OBJ = ${SRC:.c=.o}
DEP = ${OBJ:.o=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} main

-include ${DEP}

# END
