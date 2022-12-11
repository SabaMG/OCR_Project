#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "warp.h"


int main(int argc, char** argv){
	/*double M1[8][8] = {{0, 1, 2, 3, 4, 5, 6, 7},
			   {8, 9, 10, 11, 12, 13, 14, 15},
			   {16, 17, 18, 19, 20, 21, 22, 23},
			   {24, 25, 26, 27, 28, 29, 30, 31},
			   {32, 33, 34, 35, 36, 37, 38, 39},
			   {40, 41, 42, 43, 44, 45, 46, 47},
			   {48, 49, 50, 51, 52, 53, 54, 55},
			   {56, 57, 58, 59, 60, 61, 62, 63}};
	double Id[8][8] = {{1, 0, 0, 0, 0, 0, 0, 0},
			   {0, 1, 0, 0, 0, 0, 0, 0},
			   {0, 0, 1, 0, 0, 0, 0, 0},
			   {0, 0, 0, 1, 0, 0, 0, 0},
			   {0, 0, 0, 0, 1, 0, 0, 0},
			   {0, 0, 0, 0, 0, 1, 0, 0},
			   {0, 0, 0, 0, 0, 0, 1, 0},
			   {0, 0, 0, 0, 0, 0, 0, 1}};*/

	if(argc != 2)
		errx(1, "./main [IMG]\n");
	
	//Load Image as src surface
	SDL_Surface* img = IMG_Load(argv[1]);
	SDL_Surface* img_cvrt = SDL_ConvertSurfaceFormat(img,
	 SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(img);
	img = img_cvrt;
	SDL_LockSurface(img);

	//Create dest surface
	SDL_Surface* imgWarped = SDL_ConvertSurfaceFormat(img,
	 SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_LockSurface(imgWarped);
	//Clear all its pixels
	SDL_memset(imgWarped->pixels, 0, imgWarped->h*imgWarped->pitch);

	/*printf("M1 =\n");
	printMat(M1);
	double M1_t[8][8] = {};
	transposeMat(M1, M1_t);
	printf("M1_t =\n");
	printMat(M1_t);

	double tmp[8][8] = {};
	MultMat8(Id, M1_t, tmp);
	printf("Mult Id*M1_t =\n");
	printMat(tmp);

	double vect[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	double tmp2[8] = {};
	MultMat8x1(Id, vect, tmp2);
	printf("tmp2 =\n");
	for(size_t i = 0; i < 8; i++)
		printf(" %2.4f |", tmp2[i]);
	printf("\n");*/
	
	struct Rectangle A = {{360, 420, 130, 100}, {110, 270, 400, 280}};
	struct Rectangle B = {{500, 500, 100, 100}, {200, 390, 390, 200}};

	/*struct Rectangle* B = calloc(1, sizeof(struct Rectangle));
	B->px[0] = 500;
	B->px[1] = 500;
	B->px[2] = 100;
	B->px[3] = 100;
	B->py[0] = 200;
	B->py[1] = 390;
	B->py[2] = 390;
	B->py[3] = 200;*/

	//Main call
	Warp(img, imgWarped, &A, &B);	
	
	SDL_FreeSurface(imgWarped);
	SDL_FreeSurface(img);
	return 0;
}
