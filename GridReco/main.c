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

    SDL_Init(SDL_INIT_VIDEO);//SDL Init
    IMG_Init(IMG_INIT_JPG);//SDL_IMG Init

    SDL_Surface* imgOrigin = IMG_Load(argv[1]);//Load edge picture
    imgOrigin = SDL_ConvertSurfaceFormat(imgOrigin, SDL_PIXELFORMAT_BGRA32, 0);
    int W = imgOrigin->w;//Width of the edge picture
    int H = imgOrigin->h;//Height of the edge picture
   
    int maxRho = diagLen(W,H); //rho values : [-maxRho, maxRho]
    int maxTheta = 180; //theta values : [0, maxTheta]
    int nbLines = 3; //number of lines to print
    
    //Create and Fill the hough accumulator
    int* Acc = HoughAccu(imgOrigin->pixels, W, H, maxRho, maxTheta);

    //Compute and print the lines
    PrintLines(imgOrigin, nbLines, Acc, maxRho, maxTheta);




    ////Pour travailler sur les listes de pics
    /*int L_w[W];
    int L_h[H];
    for (int i = 0; i < W; i++){
        L_w[i] = 0;    
    }
    for (int i = 0; i < H; i++){
        L_h[i] = 0;    
    }
    PixelOnColsAndLines(imgOrigin->pixels, L_w, L_h, W, H);
    Uint32* dataPic = listToPic(L_w, W, H);
    SDL_Surface* PicList = SDL_CreateRGBSurfaceFrom(dataPic, W, H, 32, H*32/8, 0xff0000, 0x00ff00, 0x0000ff, 0);
    IMG_SaveJPG(PicList, "PicList_W.jpg" , 100);
    free(dataPic);
    Uint32* dataPic2 = listToPic(L_h, H, W);
    SDL_Surface* PicList2 = SDL_CreateRGBSurfaceFrom(dataPic2, H, W, 32, W*32/8, 0xff0000, 0x00ff00, 0x0000ff, 0);
    IMG_SaveJPG(PicList2, "PicList_H.jpg" , 100);
    free(dataPic2);*/

    
    //Saving picture with drawn lines
    IMG_SaveJPG(imgOrigin, argv[2], 100);
    
    //Cleaning
    free(Acc);
    SDL_FreeSurface(imgOrigin);
    
    
    return EXIT_SUCCESS;



    ///##################################
    ///##############FIN#################
    ///##################################











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
    /*int Hough_lines[diag_length][91];
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
    int angle = MaxIndex2DArray(Hough_lines, diag_length, 91);*/
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
    CutAndSaveBoxes(argv[2], coord_X_list, coord_Y_list, filename_, 10, 11);

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

    SDL_FreeSurface(img);

    //Cleaning up
    IMG_Quit();
    SDL_Quit();
    return 0;

}
