#include "tools.h"
#include <math.h>
#include "center_number.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define RAD(A)  (M_PI*((double)(A))/180.0)

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

int* houghAccumulator(Uint32* pixels, int* w, int* h, int* _nbRhos, int* _nbThetas){
    int max_rho = diagLen(*w, *h);
    //printf("DiagLength = %i\n", max_rho);
    int nbRhos = 2*max_rho;
    *_nbRhos = nbRhos;
    int max_theta = 360;
    int nbThetas = max_theta;
    *_nbThetas = nbThetas;
    int W = *w;
    int H = *h;

    int* Acc = malloc(nbRhos*nbThetas*sizeof(int));
    memset(Acc, 0, nbRhos*nbThetas*sizeof(int));
    for(int x = 0; x < W; x++){
    for(int y = 0; y < H; y++){
        if((pixels[y*W+x] & 0xff) == 0xff){
            for(int theta = 0; theta < max_theta; theta++){
                //int rho = (int)((((double)x)*cos(theta)
                // + ((double)y)*sin(theta))+.5);
                int rho = floor((((double)(x - nbRhos/2))*cos(RAD(theta))
                 + ((double)(y - nbThetas/2))*sin(RAD(theta)))+.5);
                if(rho < -max_rho || rho > max_rho) continue;
                int rho_idx = rho + max_rho;
                //Acc[theta * nbRhos + rho_idx] += 1;
                //printf("%i, %i\n", rho_idx, theta);
                Acc[rho_idx * nbThetas + theta] += 1;
            }
        }
    }
    }
    return Acc;
}

int inPic(int x, int y, int w, int h){
    return ((x >= 0 && x < w) && (y >= 0 && y < h));
}

void clearAround(int* Accu, int x, int y, int w, int h, int range){
    int from_x = x - range;
    int to_x = x + range;
    int from_y = y - range;
    int to_y = y + range;

    //printf("Clear %i,%i\n", x, y);
    //printf("from %i,%i to %i,%i\n", from_x, from_y, to_x, to_y);
    for(int i = from_x; i < to_x; i++){
        for (int j = from_y; j < to_y; j++)
        {
            if(inPic(i, j, w, h)){
                Accu[j*w + i] = 0;
            }
        }
        
    }
}

void maxInAccu(int* Accu, int* nbRhos, int* nbThetas, int* _rho, int* _theta){
    int res[] = {-1, -1};
    int max = 0;
    for(int rho = 0; rho < *nbRhos; rho++){
    for(int theta = 0; theta < *nbThetas; theta++){
        if (Accu[theta * *nbRhos + rho] > max){
            max = Accu[theta * *nbRhos + rho];
            res[0] = rho;
            res[1] = theta;
        }
    }
    }
    //Accu[res[1] * *nbRhos + res[0]] *= -1;
    *_rho = res[0];
    *_theta = res[1];
    clearAround(Accu, res[0], res[1], *nbRhos, *nbThetas, 15);
}

void displayOneLine(Uint32* pixels, int rho, int theta, int* w, int* h){
    for(int x = 0; x < *w; x++){
        int y = ((-cos(RAD(theta))*x + rho)/sin(RAD(theta)));
        //printf("GEEH : %i,%i -> %i,%i / %i\n", rho, theta, x, y, *h);
        if (y > 0 && y < *h){
            pixels[y**w + x] = (0xff << 24) | (0xff << 16) |
             (0x00 << 8) | (0x00);
        }  
    }
}

void putPixel(Uint32* pixels, int x, int y, int w){
    pixels[(y)*w + x] = (0x00 << 24) | (0x00 << 16) |
                        (0xff << 8) | (0xff);
}

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
                if (x < w && x > 0 && y > 0 && y < h){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i\n", x, y);
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
                if (x < w && x > 0 && y > 0 && y < h){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i\n", x, y);
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
                if (y < h && y > 0 && x > 0 && x < h){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i\n", x, y);
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
                if (y < h && y > 0 && x > 0 && x < h){
                    putPixel(pixels, x, y, w);
                    //printf("%i,%i\n", x, y);
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

int* FinalAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta){
    int nbRhos = maxRho;
    int nbThetas = maxTheta;
    int* Accu = malloc(nbRhos*nbThetas*sizeof(int));
    memset(Accu, 0, nbRhos*nbThetas*sizeof(int));

    for(int x = 0; x < W; x++){
    for(int y = 0; y < H; y++){
        if((pixels[y*W+x]&0xffffff) != 0xffffff) continue;
        for (int theta = 0; theta < maxTheta; theta++)
        {
            int rho = (double)x*(cos(RAD(theta))) + (double)y*(sin(RAD(theta)));
            if (rho < 0)
                rho = (double)x*(cos(RAD(theta)+M_PI)) + (double)y*(sin(RAD(theta)+M_PI));
            Accu[theta*nbRhos + rho] += 1;
        }
    }
    }
    return Accu;
}

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

int* AnotherAccu(Uint32* pixels, int* w, int* h){
    int max_theta = 180;
    int max_rho = diagLen(*w, *h);
    int* res = malloc(max_theta*max_rho*sizeof(int));
    memset(res, 0, max_rho*max_theta*sizeof(int));

    for(int x = 0; x < *w; x++){
    for(int y = 0; y < *h; y++){
        if((pixels[y**w + x]&0xff) == 0xff){
            for(int theta = 0; theta < max_theta; theta++){
                int rho = x*cos(RAD(theta)) + y*sin(RAD(theta));
                if(rho < 0 || rho > max_rho) continue;
                res[rho*max_theta + theta] += 1;
            }
        }
    }
    }
    return res;
}

int* houghtransform(Uint32 *pixels, int *w, int *h, int* _rho, int* _theta){
	int rho = 0;
	int theta = 0;
	int W = *w;
    int H = *h;
	int max_rho = *_rho;//diagLen(*w, *h)/2;
	int max_theta = *_theta;//360;
	int y = 0;
	int x = 0;

	int* hMat = malloc(max_theta*max_rho*sizeof(int));
	memset(hMat, 0, max_theta*max_rho*sizeof(int));

	for(rho = 0; rho < max_rho; rho++){
	for(theta = 0; theta < max_theta; theta++){
		double C = cos(RAD(theta));
		double S = sin(RAD(theta));
        
        int nbpix = 0;

		if (theta<45 || (theta>135 && theta<225) || theta>315){
		for(y = 0; y < H; y++){
			double dx = W/2.0 + (rho - (H/2.0-y)*S)/C;
			if (dx<0 || dx>=W) continue;
			x = (int)(dx+.5);
			if (x == W) continue;
            if ((pixels[y**w+x]&0xff) == 0xff)
			    nbpix++;
		}
		}else{
		for(x = 0; x < W; x++) {
			double dy = H/2.0 - (rho - (x - W/2.0)*C)/S;
			if ( dy < 0 || dy >= H ) continue;
			y = (int)(dy+.5);
			if (y == H) continue;
			if ((pixels[y**w+x]&0xff) == 0xff)
                nbpix++;  
		}
		}
        hMat[max_theta * y + x] = nbpix;
	}
	}
	return hMat;
}
//A-FAIRE -> Conversion polaire vers cartesien
/**
for y in range(accumulator.shape[0]):
for x in range(accumulator.shape[1]):
    if accumulator[y][x] > t_count:
    rho = rhos[y]
    theta = thetas[x]
    a = np.cos(np.deg2rad(theta))
    b = np.sin(np.deg2rad(theta))
    x0 = (a * rho) + edge_width_half
    y0 = (b * rho) + edge_height_half
    x1 = int(x0 + 1000 * (-b))
    y1 = int(y0 + 1000 * (a))
    x2 = int(x0 - 1000 * (-b))
    y2 = int(y0 - 1000 * (a))
    subplot3.plot([theta], [rho], marker='o', color="yellow")
    subplot4.add_line(mlines.Line2D([x1, x2], [y1, y2]))
**/

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

// Min function between two numbers
int min(int x, int y)
{
    if(x < y)
    {
        return x;
    }
    return y;
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
<<<<<<< HEAD
=======

>>>>>>> resize_boxes
