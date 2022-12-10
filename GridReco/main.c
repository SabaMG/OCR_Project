#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "tools.h"
#include "center_number.h"

#define RAD(A)  (M_PI*((double)(A))/180.0)

int main(int argc, char *argv[]){

    if (argc != 3)
	    errx(1, "Expected: ./main [picture] [edgeFiltered]");

    SDL_Init(SDL_INIT_VIDEO);//SDL Init
    IMG_Init(IMG_INIT_JPG);//SDL_IMG Init

    
    ///#################################
    ///#########Les Variables###########
    ///#################################

    //Load the original picture (without sobel filter)
    SDL_Surface *originImg = IMG_Load(argv[1]);
    if (originImg == NULL)
	    err(1, "Surface errors found");

    //Load the sobel picture
    SDL_Surface *sobelImage = IMG_Load(argv[2]);
    if (originImg == NULL)
	    err(1, "Surface errors found");
    SDL_Surface *tmp = SDL_ConvertSurfaceFormat(sobelImage,
     SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(sobelImage);
    sobelImage = tmp;

    char* linesImgPath = "LinesImg.jpg";
    int case_coor[81][2] = {};
    SDL_Surface boxesArray[81] = {};
    int res = Segmentation(originImg, sobelImage, linesImgPath, case_coor,
     boxesArray);

    printf("RETURN VALUE: %i\n", res);

    //print as a test
    printf("Last box W: %i\n", boxesArray[80].w);
    printf("Last box y: %i\n", case_coor[80][1]);

    return EXIT_SUCCESS;

    ///##################################
    ///##############FIN#################
    ///##################################

}
