# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFALGS =
LDLIBS = 

SRC = solver.c read_file.c backtracking.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: solver

solver: ${OBJ}

-include : ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} solver
