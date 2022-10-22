#include "tools.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//returns the index of the max in _list
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

//returns the index of the min in _list
size_t minList(size_t _list[], size_t len){
    size_t min_index = 0;
    size_t min = _list[0];
    for (size_t i = 0; i < len; i++){
        if (_list[i] < min){
            min = _list[i];
            min_index = i;
        }
    }
    return min_index;
}

//Computes diagonal length
size_t diagLen(size_t w, size_t h){
    return (size_t)(sqrt((w * w) + (h * h)));
}

//Fills Hough_lines[] with Hough lines algorithm
void HoughAngle(SDL_Surface* img, int Hough_lines[][91], size_t w_){
    Uint32 * pixels = (Uint32 *)img->pixels;
    for (int x = 0; x < img->w; x++){
        for (int y = 0; y < img->h; y++)
        {
            Uint32 pixel = pixels[y * (img->w) + x];
            
            if ((pixel & 0xff) == 0xff){
                for (size_t rho = 0; rho < w_; rho++)
                {
                    for (int theta = 0; theta < 91; theta++)
                    {
                        if((int)rho==(int)(y*cos(theta*M_PI/180) +
                        x*sin(theta*M_PI/180))){
                            Hough_lines[rho][theta] += 1;
                        }
                    }   
                }
            }
        }
    }
}

//returns the second dimension index of the max
size_t MaxIndex2DArray(int table[][91], size_t w, size_t h){
    size_t max_index = 0;
    int max = 0;
    for (size_t i = 0; i < w; i++){
        for (size_t j = 0; j < h; j++){
            if (table[i][j] > max){
                max = table[i][j];
                max_index = j;
            }
        }
    }
    return max_index;
}

//Computes the number of white pixel for each line and col
void PixelOnColsAndLines(Uint32* pixels, int linesX[], int linesY[],
 size_t lenX, size_t lenY){
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

//Puts the indexs of the 10 max of list[] in dest[] (with threshold)
void Sort10MaxsIndexs(int list[], size_t len, size_t dest[]){
    for (size_t round = 0; round < 10; round++){
        size_t max = maxList(list, len);
        size_t ind = max-1;
        int threshold = (int)((float)(list[max])*0.90);
        while (ind > 0 && (list[ind] > threshold || max - ind < 10))
        {
            list[ind] *= -1;
            ind--;
        }
        ind = max + 1;
        while (ind < len && (list[ind] > threshold || ind - max < 10))
        {
            list[ind] *= -1;
            ind++;
        }
        list[max] *= -1;
        dest[round] = max;
    }
}

//Turns detected lined and cols in red
void DisplayLines(Uint32* pixels, size_t listX[], size_t listY[], size_t ImgW,
 size_t ImgH){
    for(size_t r = 0; r < 10; r++){
        size_t cooX = listX[r];
        for (size_t j = 0; j < ImgH; j++){
            pixels[j * (ImgW) + cooX] = (0xff << 24) | (0xff << 16) |
             (0x00 << 8) | (0x00);
        }
        size_t cooY = listY[r];
        for (size_t j = 0; j < ImgW; j++){
            pixels[cooY * (ImgW) + j] = (0xff << 24) | (0xff << 16) |
             (0x00 << 8) | (0x00);
        }
    }
}

//Cuts the picture based on the intersections and saves all boxes
void CutAndSaveBoxes(char* PictPath, size_t listOfX[], size_t listOfY[],
 char* pathToSave, size_t iIndex, size_t jIndex){
    //Load the original picture (without sobel filter)
    SDL_Surface *newImg = IMG_Load(PictPath);
    for (size_t j = 0; j < 9; j++){
        for (size_t i = 0; i < 9; i++){
            SDL_Rect case_;
            case_.x = listOfX[i];
            case_.y = listOfY[j];
            case_.w = listOfX[i+1]-listOfX[i];
            case_.h = listOfY[j+1]-listOfY[j];
            SDL_Surface *resultSurf = SDL_CreateRGBSurface(0, case_.w,
             case_.h, 32, 0, 0, 0, 0);
            SDL_UnlockSurface(resultSurf);
            if (SDL_BlitSurface(newImg, &case_, resultSurf, NULL) == 0)
            {
                pathToSave[iIndex] = '0' + i;
                pathToSave[jIndex] = '0' + j;
                IMG_SaveJPG(resultSurf, pathToSave, 100);
            }
        }
    }
}