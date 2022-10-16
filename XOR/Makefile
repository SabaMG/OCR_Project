# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS = 
LDLIBS =

SRC = neural_imp.c XOR.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: XOR

XOR: ${OBJ}

-include: ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} XOR
