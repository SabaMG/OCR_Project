/* gcc main.c -I"src/include" -L"src/lib" -Wextra -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o test */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#define PI 3.14159265
#include <errno.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//#include "src/include/SDL2/SDL.h"
//#include "src/include/SDL2/SDL_image.h"

//#define main main

size_t maxList(int _list[], size_t len){
    size_t max_index = 0;
    int max = 0;
    for (size_t i = 0; i < len; i++){
        if (_list[i] > max){
            max = _list[i];
            max_index = i;
        }
    }
    return max_index;
}


int main(int argc, char *argv[]){
    
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(1) == 0)
            return 1;

    if (argc != 2)
            return 1;
    
    //SDL_Surface __Surf = *IMG_Load(argv[1]);


    SDL_Surface *img = IMG_Load(argv[1]);
    if (img == NULL)
        return 1;

    window = SDL_CreateWindow("SDL_RenderClear",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        img->w, img->h,
                        0);

    // We must call SDL_CreateRenderer in order for draw calls to affect this window.
    renderer = SDL_CreateRenderer(window, -1, 0);
    /*if (renderer == NULL)
    {
        perror ("renderer errors found");
        return 1;
    }*/

    ///SDL_Texture *background = SDL_CreateTextureFromSurface(renderer, img);
    SDL_Texture *background = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888/*img->format->format*/, SDL_TEXTUREACCESS_STATIC,
        img->w, img->h);
    if (background == NULL)
        return 1;
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);



    size_t diag_length = (size_t)(sqrt(((img->w) * (img->w))+((img->h) * (img->h))));
    printf("Diagonal_length: %zu\n", diag_length);
    int Hough_lines[diag_length][91];
    //memset( Hough_lines, 0, ((int)(diag_length))*91*sizeof(int) );
    for (size_t i = 0; i < diag_length; i++)
    {
        for (size_t j = 0; j < 91; j++)
        {
            Hough_lines[i][j] = 0;
        }
        
    }
    //printf("%i\n", Hough_lines[3][8]);

    ///Traitement gris + B&W
    Uint32 * pixels = (Uint32 *)img->pixels;
    for (int x = 0; x < img->w; x++){
        for (int y = 0; y < img->h; y++)
        {
            Uint32 pixel = pixels[y * (img->w) + x];
            ///GrayScaling
            
            Uint8 r = pixel >> 16 & 0xff;
            Uint8 g = pixel >> 8 & 0xff;
            Uint8 b = pixel & 0xff;
            Uint8 v = (r + g + b) / 3;
            //pixel = (0xff << 24) | (v << 16) | (v << 8) | v; //Gray
            pixel = (v > 100) ? (0xff << 24) | (0xff << 16) | (0xff << 8) | 0xff : 0; //B&W
            pixels[y * img->w + x] = pixel;
            
            if ((pixel & 0xff) == 0xff){
                for (size_t rho = 0; rho < diag_length; rho++)
                {
                    for (int theta = 0; theta < 91; theta++)
                    {
                        if((int)rho==(int)(y*cos(theta*M_PI/180) + x*sin(theta*M_PI/180))){
                            Hough_lines[rho][theta] += 1;
                        }
                    }   
                }
            }
        }
        //printf("%i / %i\n", x, img->w);
    }

    size_t max_index = 0;
    int max = 0;
    for (size_t i = 0; i < diag_length; i++){
        for (size_t j = 0; j < 91; j++){
            if (Hough_lines[i][j] > max){
                max = Hough_lines[i][j];
                max_index = j;
            }
        }
    }
    int angle = max_index;//(max_index - 90) * ((((int)max_index - 90) < 0) ? -1 : 1);
    printf("Angle: %i\n", angle);
    SDL_UpdateTexture(background, NULL, img->pixels,
        img->w * sizeof(Uint32));

    size_t lenX = img->w;
    size_t lenY = img->h;
    int linesX[lenX];
    int linesY[lenY];
    for (size_t i = 0; i < lenX; i++){
        linesX[i] = 0;    
    }
    for (size_t i = 0; i < lenY; i++){
        linesY[i] = 0;
    }
    if (angle == 0){
        for (size_t x = 0; x < lenX; x++){
            for (size_t y = 0; y < lenY; y++){
                Uint32 pixel = pixels[y * (lenX) + x];
                if ((pixel & 0xff) == 0xff){
                    linesX[x]++;
                    linesY[y]++;
                }
            }
        }
    }

    for (size_t round = 0; round < 10; round++){
        size_t maxX = maxList(linesX, lenX);
        for (size_t j = 0; j < lenY; j++){
            pixels[j * (lenX) + maxX] = (0xff << 24) | (0xff << 16) | (0x00 << 8) | (0x00);
            linesX[maxX] *= -1;
        }
    }
    /*for (size_t i = 0; i < 10; i++){
        size_t* Ro = 0;
        int thetA = MaxIndexInTable(Hough_lines, diag_length, Ro);
        Hough_lines[*Ro][thetA] *= -1;
        size_t x0 = (size_t)(cos(thetA)*(double)*Ro);
        printf("i: %zu x0: %zu\n", i, x0);
        for (size_t y = 0; y < (size_t)img->h; y++)
            pixels[y * (img->w) + x0] = (0xff << 24) | (0xff << 16);
    }*/

    SDL_UpdateTexture(background, NULL, img->pixels,
        img->w * sizeof(Uint32));
    
    //SDL_Texture* background = IMG_LoadTexture(renderer, argv[1]);

    // Select the color for drawing. It is set to red here.
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Clear the entire screen to our selected color.
    SDL_RenderClear(renderer);

    
    SDL_RenderCopy(renderer, background, NULL, NULL);
    /*if (SDL_RenderCopy(renderer, background, NULL, &pos) != 0){
        perror ("Copy errors found");
        return 1;
    }*/
    

    // Up until now everything was drawn behind the scenes.
    // This will show the new, red contents of the window.
    SDL_RenderPresent(renderer);

    // Give us time to see the window.
    SDL_Delay(1000);

    // Always be sure to clean up
    IMG_Quit();
    SDL_Quit();
    return 0;

}

/*
int main(int argc, char *argv[]){
    
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return 1;

    //if (argc != 2)
            //return 1;
    
    //SDL_Surface Surf = *IMG_Load(argv[1]);

    window = SDL_CreateWindow("SDL_RenderClear",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        512, 512,
                        0);

    // We must call SDL_CreateRenderer in order for draw calls to affect this window.
    renderer = SDL_CreateRenderer(window, -1, 0);

    // Select the color for drawing. It is set to red here.
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Clear the entire screen to our selected color.
    SDL_RenderClear(renderer);

    // Up until now everything was drawn behind the scenes.
    // This will show the new, red contents of the window.
    SDL_RenderPresent(renderer);

    // Give us time to see the window.
    SDL_Delay(5000);

    // Always be sure to clean up
    SDL_Quit();
    return 0;

}
*/
