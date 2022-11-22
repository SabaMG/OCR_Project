#include "tools.h"
#include "center_number.h"

#include <math.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define RAD(A)  (M_PI*((double)(A))/180.0)
#define INIT_POINTSTRUCT(X, Y) \
{ .x=X, .y=Y }


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

size_t Point_minX_index(struct Point* list, size_t start, size_t end){
    int min = list[start].x;
    size_t min_index = start;
    for(size_t i = start; i < end; i++){
        if(list[i].x < min){
            min = list[i].x;
            min_index = i;
        }
    }
    return min_index;
}

size_t Point_minY_index(struct Point* list, size_t start, size_t end){
    int min = list[start].y;
    size_t min_index = start;
    for(size_t i = start; i < end; i++){
        if(list[i].y < min){
            min = list[i].y;
            min_index = i;
        }
    }
    return min_index;
}

void Point_swap(struct Point* list, size_t i, size_t j){
    struct Point tmp = list[i];
    list[i] = list[j];
    list[j] = tmp;
}

void Point_inSort_x(struct Point* list, size_t start, size_t end){
    while (start < end - 1)
    {
        size_t ind = Point_minX_index(list, start, end);
        Point_swap(list, start, ind);
        start++;
    }
    
}

void Point_inSort_y(struct Point* list, size_t start, size_t end){
    while (start < end - 1)
    {
        size_t ind = Point_minY_index(list, start, end);
        Point_swap(list, start, ind);
        start++;
    }
    
}

void Point_sortInters(struct Point* list, size_t length, int nbLines){
    Point_inSort_y(list, 0, length);
    for (size_t line = 0; line < (size_t)nbLines/2; line++)
    {
        size_t start = line*nbLines/2;
        Point_inSort_x(list, start, start + nbLines / 2);
    }
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
    if ((pixels[(y)*w + x]& 0xffffffff) != 0xffff0000)
        pixels[(y)*w + x] = 0xffff0000;
    else
        pixels[(y)*w + x] = 0xff << 24 | 0x00 << 16 | 0xff << 8 | 0x00;
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
                    //printf("%i,%i /\n", x, y);
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
                    //printf("%i,%i /\n", x, y);
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
                    //printf("%i,%i /\n", x, y);
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
                    //printf("%i,%i /\n", x, y);
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
int PrintLines(SDL_Surface* img, int nbLines, int* Acc, int maxRho, int maxTheta, int* H_lines, int* V_lines){
    int nbRhos = 2*maxRho;//rho : [-maxRho, maxRho]
    int nbThetas = maxTheta;//theta : [0, theta]
    int W = img->w;//Width of the edge picture
    int H = img->h;//Height of the edge picture
    int i_H = 0;
    int i_V = 0;

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

        if(theta <= 60 && theta >=-60){
            //printf("OUI %i\n", theta);
            H_lines[i_H] = x1;
            H_lines[i_H+1] = y1;
            H_lines[i_H+2] = x2;
            H_lines[i_H+3] = y2;
            i_H += 4;
        }else{
            V_lines[i_V] = x1;
            V_lines[i_V+1] = y1;
            V_lines[i_V+2] = x2;
            V_lines[i_V+3] = y2;
            i_V += 4;
        }

        //Debug purposes
        printf("=== Ligne %i :\n", round+1);
        printf("r,t: %i,%i -> %i \n", rho, theta, Acc[theta*maxRho + rho]);
        //printf("x0,y0: %f, %f\n", x0, y0);
        printf("x1,y1: %i, %i\n", x1, y1);
        printf("x2,y2: %i, %i\n\n", x2, y2);

        //Draw the line between (x1,y1) and (x2,y2)
        BresenhamLine(img->pixels, x1, y1, x2, y2, W, H);
    }
    int angle = 0;
    if (nbLines > 0){
        if(theta > 90)
            angle = min(180-theta, theta-90);
        else
            angle = min(90-theta, theta);
    }
    return angle;
}

int GetIntersection(int x1_, int y1_, int x2_, int y2_, int x3_, int y3_, int x4_, int y4_, int* X, int* Y){
    if ((x1_ == x2_ && y1_ == y2_) || (x3_ == x4_ && y3_ == y4_)){
        return 0;
    }

    long long int x1 = x1_;
    long long int x2 = x2_;
    long long int x3 = x3_;
    long long int x4 = x4_;
    long long int y1 = y1_;
    long long int y2 = y2_;
    long long int y3 = y3_;
    long long int y4 = y4_;

    double denominator = ((x1 - x2)*(y3 - y4)-(y1 - y2)*(x3 - x4));

    // Lines are parallel
	if (denominator == 0)
		return 0;

    long long int numerator_x = (((x1*y2 - y1*x2)*(x3-x4))-((x1-x2)*(x3*y4 - y3*x4)));
    long long int numerator_y = ((x1*y2 - y1*x2)*(y3-y4)-(y1-y2)*(x3*y4 - y3*x4));
    *X = numerator_x / denominator;
	*Y = numerator_y / denominator;


    /*printf("denominator : %f\n", denominator);
    printf("numerator x : %lld\n", numerator_x);
    printf("numerator y : %lld\n", numerator_y);*/
	return 1;
}

struct Point* ComputeInters(int* H_lines, size_t H_len, int* V_lines, size_t V_len, int nbLines){

    struct Point* inters = calloc(nbLines/2 * nbLines/2, sizeof(struct Point));
    size_t inters_len = nbLines/2 * nbLines/2;
    int index = 0;
    for(size_t i = 0; i < H_len; i+= 4){
        for(size_t j = 0; j < V_len; j += 4){
            int X = 0;
            int Y = 0;
            if(GetIntersection(H_lines[i], H_lines[i+1], H_lines[i+2], H_lines[i+3],
                V_lines[j], V_lines[j+1], V_lines[j+2], V_lines[j+3], &X, &Y)){
                    struct Point t = INIT_POINTSTRUCT(X, Y);
                    inters[index] = t;
            }
            index++;
        }
    }
    Point_sortInters(inters, inters_len, nbLines);
    return inters;
}

void CutGrid(char* imgPath, struct Point* inters, char* pathToSave,
 size_t iIndex, size_t jIndex, size_t nbLines){
    //Load the original picture (without sobel filter)
    SDL_Surface *newImg = IMG_Load(imgPath);
    if (newImg == NULL)
	    err(1, "Surface errors found");

    /*for(size_t i = 0; i < nbLines/2 * nbLines/2; i++){
        printf("%zu: x=%4i y=%4i\n", i, inters[i].x, inters[i].y);
    }*/

    size_t nbRows = nbLines/2;

    for (size_t i = 0; i < nbRows - 1; i++){
        for (size_t j = 0; j < nbRows - 1; j++){
            SDL_Rect case_;

            case_.x = inters[j*nbRows + i].x;
            case_.y = inters[j*nbRows + i].y;
            case_.w = abs(case_.x - inters[(j + 1)*nbRows + (i+1)].x);
            case_.h = abs(case_.y - inters[(j + 1)*nbRows + (i+1)].y);

            // Center case
            ajuste_case(newImg, &case_);

            SDL_Surface *resultSurf = SDL_CreateRGBSurface(0, case_.w,
             case_.h, 32, 0, 0, 0, 0);
            SDL_UnlockSurface(resultSurf); //SegFault
            if (SDL_BlitSurface(newImg, &case_, resultSurf, NULL) == 0)
            {

                printf("case: x=%4i y=%4i\n", case_.x, case_.y);

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
