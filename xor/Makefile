# Makefile

CC = gcc-11
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -std=c99 -O0 -g
LDFLAGS =
LDLIBS =

OBJ = xor.o
DEP = ${OBJ:.o=.d}

all: xor

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} xor

-include ${DEP}

# END
