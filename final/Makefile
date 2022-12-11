# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = `pkg-config --cflags gtk+-3.0`  -Wall -Wextra -std=c99 -O3 -fsanitize=address -g
LDFLAGS = `pkg-config --libs gtk+-3.0` -lm -fsanitize=address -g
#CFLAGS = `pkg-config --cflags gtk+-3.0`  -Wall -Wextra -std=c99 -O3
#LDFLAGS = `pkg-config --libs gtk+-3.0` -lm 
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf`

SRC = main.c src/network/network.c src/network/ocr.c src/network/training.c src/utils/utils.c src/ui/ui.c src/ui/train.c src/ui/resolve.c src/solver/solver.c src/result/draw_result.c src/preprocess/sobel.c src/preprocess/tools.c src/preprocess/center_number.c src/preprocess/rotation.c src/preprocess/gauss.c src/preprocess/flood_fill.c src/utils/queue.c
OBJ = ${SRC:.c=.o}
DEP = ${OBJ:.o=.d}

all: main

main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} main

# END
