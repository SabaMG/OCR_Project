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

    if (argc != 4)
	    errx(1, "Expected: ./main [picture] [edgeFiltered] [savePath]");

    SDL_Init(SDL_INIT_VIDEO);//SDL Init
    IMG_Init(IMG_INIT_JPG);//SDL_IMG Init

    SDL_Surface* imgOrigin = IMG_Load(argv[2]);//Load edge picture
    imgOrigin = SDL_ConvertSurfaceFormat(imgOrigin, SDL_PIXELFORMAT_ARGB8888, 0);
    if(SDL_LockSurface(imgOrigin) != 0)
        printf("Unable to lock the surface");
    int W = imgOrigin->w;//Width of the edge picture
    int H = imgOrigin->h;//Height of the edge picture
   
    int maxRho = diagLen(W,H); //rho values : [-maxRho, maxRho]
    int maxTheta = 180; //theta values : [0, maxTheta]
    int nbLines = 24; //number of lines to print
    
    //Create and Fill the hough accumulator
    int* Acc = HoughAccu(imgOrigin->pixels, W, H, maxRho, maxTheta);

    /*printf("##la fameuse : \n");
    for(size_t tht = 0; tht < maxTheta; tht++)
        printf("%i %zu: %i |", 147+maxRho, tht, Acc[tht*maxRho*2 + 147 + maxRho]);
    printf("\n");*/

    //##################
    //######Part 2######
    //##################
    size_t H_len = nbLines;
    struct Line* H_lines = calloc(H_len, sizeof(struct Line));
    size_t V_len = nbLines;
    struct Line* V_lines = calloc(V_len, sizeof(struct Line));


    //Compute and print the lines
    int AngleToRotate = ComputeLines(nbLines, Acc, maxRho, maxTheta,
     H_lines, &H_len, V_lines, &V_len);
    printf("Angle To Rotate: %i\n\n", AngleToRotate);
    if (AngleToRotate != 0 /*&& !notRotated*/)
        return AngleToRotate;
    
    int removed = 0;
    printf("H_lines to remove: %zu\n", H_len - 10);
    removed += RemoveNoise(H_lines, &H_len, H_len - 10);
    printf("\nV_lines to remove: %zu\n", V_len - 10);
    removed += RemoveNoise(V_lines, &V_len, V_len - 10);
    printf("\n## %i elements removed\n", removed);
    

    if (H_len < 10)
        errx(1, "Not enough H_lines: %zu\n", H_len);
    if (V_len < 10)
        errx(1, "Not enough V_lines: %zu\n", V_len);

    PrintLines(imgOrigin, H_lines, H_len, W, H);
    PrintLines(imgOrigin, V_lines, V_len, W, H);
    //Saving picture with drawn lines
    IMG_SaveJPG(imgOrigin, argv[3], 100);
    SDL_FreeSurface(imgOrigin);

    //Get intersections
    struct Point* intersXY = ComputeInters(H_lines, V_lines);


    //Sets the directory to put the boxes into
    struct stat st;
    if (stat("./boxes", &st) == 0)
        printf("/boxes is present\n");
    else{
        if (mkdir("./boxes", 0700) == -1)
            errx(1, "Unable to create ./boxes");
        printf("/boxes has been created\n");
    }

    char filename_[] = {'b', 'o', 'x', 'e', 's', '/', 'b', 'o', 'x', '_',
     '0', '0', '.', 'j', 'p', 'g', 0};
    
    CutGrid(argv[1], intersXY, filename_, 10, 11);

    
    //Cleaning
    free(H_lines);
    free(V_lines);
    free(Acc);
    free(intersXY);
    
    
    return EXIT_SUCCESS;



    ///##################################
    ///##############FIN#################
    ///##################################










    /*
    //To display the grid -> 1
    int display = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture *background;

    //Checking number of args
    if (argc != 3)
	    errx(1, "args errors found : 2 expected");

    //Init SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(1) == 0)
	    errx(1, "renderer errors found");

    //Loading the image as a surface
    SDL_Surface *img = IMG_Load(argv[1]);
    if (img == NULL)
	    errx(1, "Surface errors found");

    //Standardizes the formats
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);


    //Calculate diagonal length for compute vision
    size_t diag_length = diagLen(img->w, img->h);
    printf("Diagonal_length: %zu\n", diag_length);

    //Init Hough array[rho][theta]
    int Hough_lines[diag_length][91];
    for (size_t i = 0; i < diag_length; i++)
    {
        for (size_t j = 0; j < 91; j++)
        {
            Hough_lines[i][j] = 0;
        }
        
    }

    //Fills the hough array
    HoughAngle(img, Hough_lines, diag_length);

    //Rotation angle = theta coordinate of max value
    int angle = MaxIndex2DArray(Hough_lines, diag_length, 91);
    int angle = 0;
    printf("Angle: %i\n", angle);

    //Init lists to detect lines
    size_t lenX = img->w;
    size_t lenY = img->h;
    int linesX[lenX];
    int linesY[lenY];
    
    //Init temporary lists to sort the 10 max values
    size_t tmp_X[10] = {};
    size_t tmp_Y[10] = {};
    for (size_t i = 0; i < lenX; i++){
        linesX[i] = 0;    
    }
    for (size_t i = 0; i < lenY; i++){
        linesY[i] = 0;
    }
    
    //if grid is straight, fills the lists linesX[] and linesY[]
    if (angle == 0){
        PixelOnColsAndLines(img->pixels, linesX, linesY, lenX, lenY);
    }
   
    //Puts the indexs of the 10 maxs in tmpX[] and tmpY[]
    Sort10MaxsIndexs(linesX, lenX, tmp_X);
    Sort10MaxsIndexs(linesY, lenY, tmp_Y);


    //Fills the coordinates lists with the sorted maxs
    size_t coord_X_list[10] = {};
    size_t coord_Y_list[10] = {};
    for (size_t i = 0; i < 10; i++)
    {
        size_t tmp = minList(tmp_X, 10);
        coord_X_list[i] = tmp_X[tmp];
        tmp_X[tmp] = lenX;
        tmp = minList(tmp_Y, 10);
        coord_Y_list[i] = tmp_Y[tmp];
        tmp_Y[tmp] = lenY;
    }
    
    //Sets the directory to put the boxes into
    struct stat st;
    if (stat("./boxes", &st) == 0)
        printf("/boxes is present\n");
    else{
        if (mkdir("./boxes", 0700) == -1)
            errx(1, "Unable to create ./boxes");
        printf("/boxes has been created\n");
    }
    //Path to save the sudoku boxes
    char filename_[] = {'b', 'o', 'x', 'e', 's', '/', 'b', 'o', 'x', '_',
     '0', '0', '.', 'j', 'p', 'g', 0};
    //Cuting and saving them
    //CutAndSaveBoxes(argv[2], coord_X_list, coord_Y_list, filename_, 10, 11);

    //Rendering if display is true
    if (display){
        DisplayLines(img->pixels, coord_X_list, coord_Y_list, lenX, lenY);

        //Opens a window
        window = SDL_CreateWindow("SDL_RenderClear",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            img->w, img->h,
                            0);
        if (window == NULL)
            errx(1, "window error found");

        //In order for draw calls to affect this window
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer == NULL)
            errx(1, "renderer errors found");

        //Init Texture
        background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC, img->w, img->h);
        if (background == NULL)
            return 1;

        //Puts new red pixels in the Texture
        SDL_UpdateTexture(background, NULL, img->pixels,
            img->w * sizeof(Uint32));
        
        //Copies from texture to renderer
        SDL_RenderCopy(renderer, background, NULL, NULL);

        //Displays
        SDL_RenderPresent(renderer);
        SDL_Delay(2000);

        //Cleaning
        SDL_DestroyTexture(background);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    //SDL_FreeSurface(img);

    //Cleaning up
    IMG_Quit();
    SDL_Quit();
    return 0;*/

}
