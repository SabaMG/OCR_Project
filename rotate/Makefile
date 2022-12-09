all:
	gcc -Wall -Wextra rotation.c -o main -lm -lSDL2 -lSDL2_image

debug:
	gcc -Wall -Wextra -g -fsanitize=address rotation.c -o main -lm -lSDL2 -lSDL2_image

clean:
	rm main
