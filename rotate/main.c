#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include "rotation.c"

SDL_Surface *load_image(char *path)
{
	SDL_Surface *image = IMG_Load(path);
	if (image == NULL)
		errx(3, "IMG_Load: %s", IMG_GetError());
	return image;
}


int main(int argc, char **argv)
{
	if (argc != 2)
		errx(EXIT_FAILURE, "Usage: image-file");

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_Surface *img = load_image(argv[1]);

	double nombre = 0;
	printf("Enter a value for the angle : ");
	scanf("%le", &nombre);

	img = SDL_RotationCentral(img, nombre);
	IMG_SaveJPG(img, "out.jpg", 100);


	return EXIT_SUCCESS;
}
