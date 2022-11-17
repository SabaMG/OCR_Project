#include "tools.h"
#include "center_number.h"

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define RAD(A)  (M_PI*((double)(A))/180.0)

// Min function between two numbers
int min(int x, int y)
{
    return (x < y) ? x : y;
}

// Max function between two numbers
int max(int x, int y)
{
    return (x > y) ? x : y;
}

//returns the INDEX of the max in _list
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

//returns the INDEX of the min in _list
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
int diagLen(int w, int h){
    return (int)(sqrt((w * w) + (h * h)));
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


//Test if (x,y) coordinates are in the picture
int inPic(int x, int y, int w, int h){
    return ((x >= 0 && x < w) && (y >= 0 && y < h));
}

//clears all pixels (sets to 0) in 'range' around (x,y)
void clearAround(int* Accu, int x, int y, int w, int h, int range){
    int from_x = x - range;
    int to_x = x + range;
    int from_y = y - range;
    int to_y = y + range;

    for(int i = from_x; i < to_x; i++){
        for (int j = from_y; j < to_y; j++)
        {
            if(inPic(i, j, w, h)){
                Accu[j*w + i] = 0;
            }
        }
        
    }
}

//Fills 'rho' and 'theta' with the coordinates of the max in the accumulator
void maxInAccu(int* Accu, int nbRhos, int nbThetas, int* _rho, int* _theta){
    int res[] = {-1, -1};
    int max = 0;
    for(int rho = 0; rho < nbRhos; rho++){
    for(int theta = 0; theta < nbThetas; theta++){
        if (Accu[theta * nbRhos + rho] > max){
            max = Accu[theta * nbRhos + rho];
            res[0] = rho;
            res[1] = theta;
        }
    }
    }
    *_rho = res[0] - nbRhos/2;
    *_theta = res[1];
    clearAround(Accu, res[0], res[1], nbRhos, nbThetas, 15);
}

//put a BLUE pixel
void putPixel(Uint32* pixels, int x, int y, int w){
    pixels[(y)*w + x] = (0x00 << 24) | (0x00 << 16) |
                        (0xff << 8) | (0xff);
}

//Sub-function to draw a line
void _BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w, int h){
    int x, y, dx, dy;
    double e;
    int e_10, e_01;
    dy = abs(y2 - y1);
    dx = abs(x2 - x1);
    if(dx > dy){
        //printf("1st\n");
        if (y2 < y1){
            //printf("inverting\n");

            y = y1;
            e = 0.5*dx;
            e_10 = -dy;
            e_01 = dx;
            for(int x = x1; x < x2; x++){
                //printf("%i,%i\n", x, y);
                if (inPic(x, y, w, h)){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i // %i,%i\n", x, y, w, h);
                }
                e = e + e_10;
                if (e <= 0){
                    y -= 1;
                    e = e + e_01;
                }
            }
        }else{
            y = y1;
            e = -0.5*dx;
            e_10 = dy;
            e_01 = -dx;
            //printf("%i,%i\n", x1, x2);
            for(int x = x1; x < x2; x++){
                //printf("%i,%i\n", x, y);
                if (inPic(x, y, w, h)){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i // %i,%i\n", x, y, w, h);
                }
                e = e + e_10;
                if (e >= 0){
                    y += 1;
                    e = e + e_01;
                }
            }
        }
    }else{
        //printf("2nd\n");
        if (x2 < x1){
            //printf("inverting\n");

            x = x1;
            e = 0.5*dy;
            e_10 = -dx;
            e_01 = dy;
            for(int y = y1; y < y2; y++){
                //printf("%i,%i\n", x, y);
                if (inPic(x, y, w, h)){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i // %i,%i\n", x, y, w, h);
                }
                e = e + e_10;
                if (e <= 0){
                    x -= 1;
                    e = e + e_01;
                }
            }
        }else{
            x = x1;
            e = -0.5*dy;
            e_10 = dx;
            e_01 = -dy;
            //printf("%i,%i\n", y1, y2);
            for(int y = y1; y < y2; y++){
                //printf("%i,%i\n", x, y);
                if (inPic(x, y, w, h)){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i // %i,%i\n", x, y, w, h);
                }
                e = e + e_10;
                if (e >= 0){
                    x += 1;
                    e = e + e_01;
                }
            }
        }
    }
}

//Draw a line
void BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w, int h){
    if (abs(y2 - y1) < abs(x2 - x1)){
        if (x1 > x2)
            _BresenhamLine(pixels, x2, y2, x1, y1, w, h);
        else
            _BresenhamLine(pixels, x1, y1, x2, y2, w, h);
    }else{
        if (y1 > y2)
            _BresenhamLine(pixels, x2, y2, x1, y1, w, h);
        else
            _BresenhamLine(pixels, x1, y1, x2, y2, w, h);
    }
}

//Hough accumulator
int* HoughAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta){
    int nbRhos = 2*maxRho;
    int nbThetas = maxTheta;
    int* Accu = malloc(nbRhos*nbThetas*sizeof(int));
    memset(Accu, 0, nbRhos*nbThetas*sizeof(int));

    for(int x = 0; x < W; x++){
    for(int y = 0; y < H; y++){
        if((pixels[y*W+x]&0xffffff) != 0xffffff) continue;
        for (int theta = 0; theta < maxTheta; theta++)
        {
            int rho = (double)x*(cos(RAD(theta))) + (double)y*(sin(RAD(theta)));
            rho += maxRho;
            Accu[theta*nbRhos + rho] += 1;
        }
    }
    }
    return Accu;
}

//Computes coordinates and draw the corresponding lines
void PrintLines(SDL_Surface* img, int nbLines, int* Acc, int maxRho, int maxTheta){
    int nbRhos = 2*maxRho;//rho : [-maxRho, maxRho]
    int nbThetas = maxTheta;//theta : [0, theta]
    int W = img->w;//Width of the edge picture
    int H = img->h;//Height of the edge picture

    int baseLength = maxRho;//influencing the length of the lines drawn

    int rho = -1;//value to be fill by MaxInAccu()
    int theta = -1;//value to be fill by MaxInAccu()
    for(int round = 0; round < nbLines; round++){
        
        //Modifies 'rho' and 'theta' with the max coordinates
        maxInAccu(Acc, nbRhos, nbThetas, &rho, &theta);

        //Computing coordinates from (rho, theta) couple
        double a = cos(RAD(theta));
        double b = sin(RAD(theta));
        double x0 = (a*(double)rho);
        double y0 = (b*(double)rho);
        int x1 = (int)(x0 + baseLength * (-b));
        int y1 = (int)(y0 + baseLength * a);
        int x2 = (int)(x0 - baseLength * (-b));
        int y2 = (int)(y0 - baseLength * a);

        //Debug purposes
        printf("=== Ligne %i :\n", round+1);
        printf("r,t: %i,%i -> %i \n", rho, theta, Acc[theta*maxRho + rho]);
        printf("x0,y0: %f, %f\n", x0, y0);
        printf("x1,y1: %i, %i\n", x1, y1);
        printf("x2,y2: %i, %i\n\n", x2, y2);

        //Draw the line between (x1,y1) and (x2,y2)
        BresenhamLine(img->pixels, x1, y1, x2, y2, W, H);
    }
}

//Converts a list to a picture
Uint32* listToPic(int* listIndex, int length, int w){
    Uint32* res = malloc(length*w*sizeof(Uint32));
    memset(res, 0, length*w*sizeof(Uint32));

    for(int i = 0; i < length; i++){
        int tmp = listIndex[i];
        for(int j = 0; j < tmp; j++){
            putPixel(res, i, w-j-1, length);
        }
    }
    return res;
}

//Converts accumulator to picture
Uint32* accToPic(int* Acc, int* w, int* h){
    Uint32* res = malloc(*w * *h * sizeof(Uint32));
    memset(res, 0, *w**h*sizeof(Uint32));

    int maxMat = 0;
    for(int x = 0; x < *w; x++){
    for(int y = 0; y < *h; y++){
        if(Acc[y**w + x] > maxMat){
            maxMat = Acc[y**w + x];
            //printf("Max rho,theta : %i, %i\n", x, y);
        }
    }
    }
    
    for(int x = 0; x < *w; x++){
    for(int y = 0; y < *h; y++){
        if(Acc[y**w + x] > 0){
            int mid = (int)((int)(((double)(Acc[y**w + x])/maxMat)*255)& 0xff);
            res[y**w+x] = (0xff << 24) | (mid << 16) |
             (mid << 8) | (mid);
        }
    }
    }
    return res;
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
        int threshold = (int)((float)(list[max])*0.98);
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
            int min_val = min(listOfX[i + 1] - listOfX[i], listOfY[i + 1] - listOfY[i]);
            case_.x = listOfX[i];
            case_.y = listOfY[j];
            case_.w = min_val;
            case_.h = min_val;
            
            // Center case
            ajuste_case(newImg, &case_);

            SDL_Surface *resultSurf = SDL_CreateRGBSurface(0, case_.w,
             case_.h, 32, 0, 0, 0, 0);
            SDL_UnlockSurface(resultSurf);
            if (SDL_BlitSurface(newImg, &case_, resultSurf, NULL) == 0)
            {

                //make zoom of the surface to reich 28x28 pixels
                float z =  28 / (float)(resultSurf->w);
                resultSurf = resize(resultSurf, z);

                pathToSave[iIndex] = '0' + i;
                pathToSave[jIndex] = '0' + j;

                IMG_SaveJPG(resultSurf, pathToSave, 100);
            }
        }
    }
}
