#include <math.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "rotation.h"

// load image
SDL_Surface *load_image(char *path)
{
	SDL_Surface *image = IMG_Load(path);
	if (image == NULL)
		errx(3, "IMG_Load: %s", IMG_GetError());
	return image;
}


int main(int argc, char **argv)
{
	// Checks the number of arguments.
	if (argc != 3)
		errx(EXIT_FAILURE, "Usage: image-in image-out"); // Loock if there are any error.

	// Initialize the SDL.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError()); // Loock if there are any error.

	SDL_Surface *img = load_image(argv[1]);

	//// Just for test
	double nombre = 0;
	printf("Enter a value for the angle : ");
	scanf("%le", &nombre);
	//
	// Save
	img = RotationCentral(img, nombre);

	IMG_SaveJPG(img, argv[2], 100);
	// make rotation
	// - Convert the surface into binarize sauvala ....
	// surface_to_grayscale(surface_color);

	return EXIT_SUCCESS;
}
