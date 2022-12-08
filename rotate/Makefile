all:
	gcc -Wall -Wextra main.c rotation.c -o main -lm -lSDL2 -lSDL2_image -lSDL2_gfx

debug:
	gcc -Wall -Wextra -g -fsanitize=address main.c rotation.c -o main -lm -lSDL2 -lSDL2_image -lSDL2_gfx

clean:
	rm main
