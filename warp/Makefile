all:
	gcc -Wall -Wextra main.c warp.c -o main -lm -lSDL2 -lSDL2_image

debug:
	gcc -Wall -Wextra -fsanitize=address -g main.c warp.c -o main -lm -lSDL2 -lSDL2_image
