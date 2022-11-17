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

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    SDL_Surface* imgOrigin = IMG_Load(argv[1]);
    imgOrigin = SDL_ConvertSurfaceFormat(imgOrigin, SDL_PIXELFORMAT_BGRA32, 0);
    int W = imgOrigin->w;
    int H = imgOrigin->h;
    //int nbRhos = diagLen(W, H);
    //int nbThetas = 360;
    //int* Acc = houghAccumulator(imgOrigin->pixels, &(imgOrigin->w),
    // &(imgOrigin->h), &nbRhos, &nbThetas);
    int maxRho = diagLen(W,H);
    int maxTheta = 180;
    //int* Acc = houghtransform(imgOrigin->pixels, &(imgOrigin->w),
    // &(imgOrigin->h), &maxRho, &maxTheta);
    int* Acc = FinalAccu(imgOrigin->pixels, W, H, maxRho, maxTheta);
    /*Uint32* dataPic = accToPic(Acc, &tmp2, &tmp1);
    SDL_Surface* houghImg = SDL_CreateRGBSurfaceFrom(dataPic, tmp2, tmp1, 32, tmp2*32/8, 0xff0000, 0x00ff00, 0x0000ff, 0);
    IMG_SaveJPG(houghImg, argv[2], 100);*/

    int L_w[W];
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
    free(dataPic2);

    int rho = -1;
    int theta = -1;
    for(int round = 0; round < 5; round++){
        maxInAccu(Acc, &maxRho, &maxTheta, &rho, &theta);
        if (theta > 135)
            rho *= -1;
        double a = cos(RAD(theta));
        double b = sin(RAD(theta));
        double x0 = (a*(double)rho);
        double y0 = (b*(double)rho);
        int x1 = (int)(x0 + 1000 * (-b));
        int y1 = (int)(y0 + 1000 * a);
        int x2 = (int)(x0 - 1000 * (-b));
        int y2 = (int)(y0 - 1000* a);
        printf("r,t: %i,%i -> %i \n", rho, theta, Acc[theta*maxRho + rho]);
        /*for(int i = rho-5; i < rho + 5; i++){
            for(int j = theta-5; j < theta+5; j++){
                printf("##%i,%i -> %i && %i && %i && %i :", i, j, (i > 0),(i < maxRho),
                (j > 0), (j < maxTheta));
                if (((i > 0) && (i < maxRho)) && ((j > 0) && (j < maxTheta)))
                    printf("%i\n",Acc[j * maxRho + j]);
            }
        }*/
        printf("x0,y0: %f, %f\n", x0, y0);
        printf("x1,y1: %i, %i\n", x1, y1);
        printf("x2,y2: %i, %i\n", x2, y2);
        BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);
    }
    ////TEST BresenhamLine
    //unary
    /*int x1 = -254;
    int y1 = -1021;
    int x2 = 396;
    int y2 = 869;
    BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);*/
    //global
    /*int x1 = 0;
    int y1 = 0;
    int x2 = W;
    int y2 = H;
    BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);
    x1 = 0;
    y1 = H;
    x2 = W;
    y2 = 0;
    BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);
    x1 = W/2;
    y1 = H;
    x2 = W/2;
    y2 = 0;
    BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);
    x1 = 0;
    y1 = H/2;
    x2 = W;
    y2 = H/2;
    BresenhamLine(imgOrigin->pixels, x1, y1, x2, y2, W, H);*/

    IMG_SaveJPG(imgOrigin, argv[2], 100);
    
    /*for(int theta = 0; theta < maxTheta; theta++){
    for(int rho = 0; rho < maxRho; rho++){
        if (Acc[theta*maxRho+rho] == 0)
            printf("  ,");
        else
            printf("%2i,", Acc[theta*maxRho+rho]);
    }
    printf("\n");
    }*/

    //Uint32* dataPic = accToPic(Acc, &maxRho, &maxTheta);
    //SDL_Surface* houghImg = SDL_CreateRGBSurfaceFrom(dataPic, nbRhos, nbThetas, 32, nbRhos*32/8, 0xff0000, 0x00ff00, 0x0000ff, 0);
    //IMG_SaveJPG(houghImg, argv[2], 100);
    //free(dataPic);
    //SDL_FreeSurface(houghImg);

    free(Acc);
    SDL_FreeSurface(imgOrigin);
    return EXIT_SUCCESS;






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
