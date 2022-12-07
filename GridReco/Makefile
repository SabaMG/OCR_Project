
all:
	gcc -Wall -Wextra main.c tools.c center_number.c -o main -lm -lSDL2 -lSDL2_image

debug:
	gcc -Wall -Wextra main.c tools.c center_number.c -o main -g -fsanitize=address -lm -lSDL2 -lSDL2_image

clean:
	rm main
	rm -rf boxes
