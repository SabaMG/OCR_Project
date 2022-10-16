# Makefile

CC = gcc-11
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99 -O0 -g
LDFLAGS =
LDLIBS =

SRC = main.c train.c utils.c
OBJ = ${SRC:.c=.o}
DEP = ${OBJ:.o=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} main

-include ${DEP}

# END
