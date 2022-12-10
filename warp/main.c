#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "warp.h"
#include "mat.h"


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
	SDL_Surface* img = IMG_Load(argv[1]);
	SDL_Surface* img_cvrt = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(img);
	img = img_cvrt;
	SDL_LockSurface(img);
	SDL_Surface* cp = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_LockSurface(cp);
	Uint32* pixels = img->pixels;
	Uint32* pixels_cp = cp->pixels;
	
	SDL_memset(img->pixels, 0, img->h*img->pitch);

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
	
	struct Rectangle* A = calloc(1, sizeof(struct Rectangle));
	A->px[0] = 360;
	A->px[1] = 420;
	A->px[2] = 130;
	A->px[3] = 100;
	A->py[0] = 110;
	A->py[1] = 270;
	A->py[2] = 400;
	A->py[3] = 280;
	struct Rectangle* B = calloc(1, sizeof(struct Rectangle));
	B->px[0] = 500;
	B->px[1] = 500;
	B->px[2] = 100;
	B->px[3] = 100;
	B->py[0] = 200;
	B->py[1] = 390;
	B->py[2] = 390;
	B->py[3] = 200;

	double H[3][3] = {};
	computeH(*A, *B, H);
	
	for(int _x = 0; _x < cp->w; _x++){
	for(int _y = 0; _y < cp->h; _y++){
		int x = _x;
		int y = /*cp->h -*/ _y;
		double X[3] = {x, y, 1};
		double tmp[3] = {};
		MultMat3x1(H, X, tmp);
		int x2 = (int)(tmp[0]/tmp[2]);
		int y2 = /*cp->h -*/ (int)(tmp[1]/tmp[2]);
		//printf("%lf, %lf, %lf\n", tmp[0], tmp[1], tmp[2]);
		//printf("%i,%i --> %i,%i\n", x, y, x2, y2);
		if(((x2>=0)&&(x2<img->w))&&(y2>=0)&&(y2<img->h)){
			Uint32 pixel = pixels_cp[y * cp->w + x];
			Uint8 r = pixel >> 16 & 0xFF;
                    	Uint8 g = pixel >> 8 & 0xFF;
                    	Uint8 b = pixel & 0xFF;
			pixels[y2*img->w+x2] = (0xff << 24)|(r << 16)|(g << 8)|(b);
		}
	}
	}
	IMG_SaveJPG(img, "res.jpg", 100);
	
	free(A);
	free(B);
	SDL_FreeSurface(cp);
	SDL_FreeSurface(img);
	return 0;
}
