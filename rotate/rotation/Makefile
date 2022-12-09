# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDLIBS = -lm `pkg-config --libs sdl2 SDL2_image`


all: main rotation

main: main.o

clean:
	${RM} *.o 
	${RM} rotation
	${RM} *.bmp
# END
