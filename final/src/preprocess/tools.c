#include "tools.h"
#include "center_number.h"

#include <math.h>
#include <err.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
///// Rajoute par charles pour ubuntu
# define M_PI           3.14159265358979323846  /* pi */
//////
#define RAD(A)  (M_PI*((double)(A))/180.0)
#define INIT_POINTSTRUCT(X, Y) \
{ .x=X, .y=Y }
#define CLEAR_AROUND 15
#define NB_LINES 20


// Min function between two numbers
int min(int x, int y)
{
    return (x < y) ? x : y;
}

//Return the index of the point with the minimal x
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

//Return the index of the point with the minimal y
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

//Swap the points at the i and j indexs
void Point_swap(struct Point* list, size_t i, size_t j){
    struct Point tmp = list[i];
    list[i] = list[j];
    list[j] = tmp;
}

//Insertion sort based on x values
void Point_inSort_x(struct Point* list, size_t start, size_t end){
    while (start < end - 1)
    {
        size_t ind = Point_minX_index(list, start, end);
        Point_swap(list, start, ind);
        start++;
    }
    
}

//Insertion sort based on y values
void Point_inSort_y(struct Point* list, size_t start, size_t end){
    while (start < end - 1)
    {
        size_t ind = Point_minY_index(list, start, end);
        Point_swap(list, start, ind);
        start++;
    }
    
}

//Sort intersections based on y then x
void Point_sortInters(struct Point* list, size_t length, int nbLines){
    Point_inSort_y(list, 0, length);
    for (size_t line = 0; line < (size_t)nbLines/2; line++)
    {
        size_t start = line*nbLines/2;
        Point_inSort_x(list, start, start + nbLines / 2);
    }
}

//Return the index of the max in list
size_t maxList_(size_t _list[], size_t start, size_t end){
    size_t min_index = start;
    size_t min = _list[start];
    for (size_t i = start; i < end; i++){
        if (_list[i] > min){
            min = _list[i];
            min_index = i;
        }
    }
    return min_index;
}

//Return the index of the minimal rho in list
size_t minList_Line(struct Line* list, size_t start, size_t end){
    size_t min_index = start;
    int min = abs(list[start].rho);
    for (size_t i = start; i < end; i++){
        if (abs(list[i].rho) < min){
            min = list[i].rho;
            min_index = i;
        }
    }
    return min_index;
}

//Swap the lines at the i and j indexs
void swap_Line(struct Line* list, size_t i, size_t j){
    struct Line tmp = list[i];
    list[i] = list[j];
    list[j] = tmp;
}

//Return the average value of the list
int AverageOfList(size_t* list, size_t length){
    int sum = 0;
    for (size_t i = 0; i < length; i++)
    {
        sum += list[i];
    }
    return sum/length;
}

//Pop the index-element by putting it at the end and decreasing the length
void list_pop(struct Line* list, size_t length, size_t index){
    while(index < length - 1){
        list[index] = list[index + 1];
        index++;
    }
    list[length - 1].rho = -1;
}

//Sort lines based on abs(rho) values
void Line_sort(struct Line* list, size_t start, size_t end){
    while (start < end - 1){
        size_t ind = minList_Line(list, start, end);
        swap_Line(list, start, ind);
        start++;
    }
}

//Computes diagonal length
int diagLen(int w, int h){
    return (int)(sqrt((w * w) + (h * h)));
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

    for(int _x = from_x; _x < to_x; _x++){
        for (int _y = from_y; _y < to_y; _y++)
        {
            int i = (_x < 0) ? _x + w : _x % w;
            int j = (_y < 0) ? _y + h : _y % h;
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

    printf("Max (%i,%i) = %i ||||\n", res[0], res[1],
     Accu[res[1] * nbRhos + res[0]]);

    *_rho = res[0] - nbRhos/2;
    *_theta = res[1];
    clearAround(Accu, res[0], res[1], nbRhos, nbThetas, CLEAR_AROUND);
}

//put a BLUE pixel
void putPixel(Uint32* pixels, int x, int y, int w){
    pixels[(y)*w + x] = 0xffff0000;
}

//Sub-function to draw a line
void _BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w,
 int h){
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

//Draw a line using Bresenham algorithm
void BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w,
 int h){
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

//Fill the Hough accumulator
int* HoughAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta){
    int nbRhos = 2*maxRho;
    int nbThetas = maxTheta;
    int* Accu = calloc(nbRhos*nbThetas, sizeof(int));

    for(int x = 0; x < W; x++){
    for(int y = 0; y < H; y++){
        if((pixels[y*W+x]&0xffffff) != 0xffffff) continue;
        for (int theta = 0; theta < maxTheta; theta++)
        {
            int rho = (double)x*(cos(RAD(theta)))+(double)y*(sin(RAD(theta)));
            rho += maxRho;
            Accu[theta*nbRhos + rho] += 1;
        }
    }
    }
    return Accu;
}

//Print the lines from list in img surface
void PrintLines(SDL_Surface* img, struct Line* list, size_t length, int img_w,
 int img_h){
    for(size_t i = 0; i < length; i++){

        struct Line line = list[i];
        //Draw the line between (x1,y1) and (x2,y2)
        BresenhamLine(img->pixels, line.x1, line.y1,
         line.x2, line.y2, img_w, img_h);

    }
}

//Compute the angle to rotate based on Horizontal lines
int AngleToRot(struct Line* H_lines, size_t* H_len){
    int H_Angle = 0;
    for(size_t i_H = 0; i_H < *H_len; i_H++){
        H_Angle += H_lines[i_H].theta;
    }
    H_Angle /= *H_len;
    return (H_Angle-90);
}

//Computes coordinates and draw the corresponding lines
int ComputeLines(int nbLines, int* Acc, int maxRho,
 int maxTheta, struct Line* H_lines, size_t* H_len, struct Line* V_lines,
  size_t* V_len){

    int nbRhos = 2*maxRho;//rho : [-maxRho, maxRho]
    int nbThetas = maxTheta;//theta : [0, theta]

    size_t i_H = 0;
    size_t i_V = 0;

    int baseLength = maxRho;//influencing the length of the lines drawn

    int rho = -1;//value to be fill by MaxInAccu()
    int theta = -1;//value to be fill by MaxInAccu()
    for(int round = 0; round < nbLines; round++){
    //while((i_H < 12 || i_V < 12)){
        
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

        struct Line new_line = {rho, theta, x1, y1, x2, y2};     

        if((theta <= 45 && theta >=-45) || (theta <= 225 && theta >=135)){
            if (i_V < *V_len){
                V_lines[i_V] = new_line;
                i_V++;
            }
        }else{
            if (i_H < *H_len){
                H_lines[i_H] = new_line;
                i_H++;
            }
        }

        //Debug purposes
        printf("=== Ligne %zu :\n", i_H + i_V);
        printf("r,t: %i,%i\n", rho, theta);
        printf("x0,y0: %f, %f\n", x0, y0);
        printf("x1,y1: %i, %i\n", x1, y1);
        printf("x2,y2: %i, %i\n\n", x2, y2);

    }

    *H_len = i_H;
    *V_len = i_V;

    return AngleToRot(H_lines, H_len);
}

//Compute the intersection point between the ((x1,y1),(x2,y2))
//and ((x3,y3),(x4,y4)) lines
int GetIntersection(int x1_, int y1_, int x2_, int y2_, int x3_, int y3_,
 int x4_, int y4_, int* X, int* Y){
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

    long long int numerator_x = (((x1*y2 - y1*x2)*(x3-x4))
    -((x1-x2)*(x3*y4 - y3*x4)));
    long long int numerator_y = ((x1*y2 - y1*x2)*(y3-y4)
    -(y1-y2)*(x3*y4 - y3*x4));
    *X = numerator_x / denominator;
	*Y = numerator_y / denominator;


    /*printf("denominator : %f\n", denominator);
    printf("numerator x : %lld\n", numerator_x);
    printf("numerator y : %lld\n", numerator_y);*/
	return 1;
}

struct Point* ComputeInters(struct Line* H_lines, struct Line* V_lines){

    size_t inters_len = NB_LINES/2 * NB_LINES/2;
    struct Point* inters = calloc(inters_len, sizeof(struct Point));
    
    int index = 0;
    for(size_t i = 0; i < 10; i++){
        for(size_t j = 0; j < 10; j++){
            int X = 0;
            int Y = 0;
            if(GetIntersection(H_lines[i].x1, H_lines[i].y1, H_lines[i].x2,
             H_lines[i].y2, V_lines[j].x1, V_lines[j].y1, V_lines[j].x2,
              V_lines[j].y2, &X, &Y)){
                    struct Point t = INIT_POINTSTRUCT(X, Y);
                    inters[index] = t;
            }
            index++;
        }
    }
    Point_sortInters(inters, inters_len, NB_LINES);
    return inters;
}

//Cut the boxes of the grid
void CutGrid(SDL_Surface* originImg, struct Point* inters, char* pathToSave,
 int case_coor[81][2], SDL_Surface boxesArray[81], size_t iIndex,
  size_t jIndex){

    /*for(size_t i = 0; i < NB_LINES/2 * NB_LINES/2; i++){
        printf("%zu: x=%4i y=%4i\n", i, inters[i].x, inters[i].y);
    }*/

    size_t nbRows = NB_LINES/2;

    for (size_t i = 0; i < nbRows - 1; i++){
        for (size_t j = 0; j < nbRows - 1; j++){
            
            
            SDL_Rect case_;
            case_.x = inters[j*nbRows + i].x;
            case_.y = inters[j*nbRows + i].y;
            int min_val = min(abs(case_.x - inters[(j + 1)*nbRows + (i+1)].x),
             abs(case_.y - inters[(j + 1)*nbRows + (i+1)].y));
            case_.w = min_val;
            case_.h = min_val;

            // Center case
            ajuste_case(originImg, &case_);

            SDL_Surface *resultSurf = SDL_CreateRGBSurface(0, case_.w,
             case_.h, 32, 0, 0, 0, 0);
            SDL_UnlockSurface(resultSurf); //SegFault
            if (SDL_BlitSurface(originImg, &case_, resultSurf, NULL) == 0) {

                //printf("case %zu: x=%4i y=%4i => w=%4i h=%4i\n",
                // i*(nbRows-1)+j, case_.x, case_.y, case_.w, case_.h);

                if(case_.w == 0 || case_.h == 0){
                    printf("Cut issue\n");
                    continue;
                }

                //make zoom of the surface to reich 28x28 pixels
                float z =  28 / (float)(resultSurf->w - 1);
                SDL_Surface* sizeSurf = resize(resultSurf, z);
                sizeSurf->w -= sizeSurf->w - 28;
                sizeSurf->h -= sizeSurf->h - 28;
                //SDL_FreeSurface(resultSurf);
                //printf("w = %i, h = %i\n", sizeSurf->w, sizeSurf->h);

                pathToSave[iIndex] = '0' + i;
                pathToSave[jIndex] = '0' + j;

                size_t ind = i*(nbRows-1)+j;
                case_coor[ind][0] = case_.x;
                case_coor[ind][1] = case_.y;
                boxesArray[ind] = *sizeSurf;

                if (IMG_SaveJPG(sizeSurf, pathToSave, 100) == -1)
                    printf("Unable to save the picture\n");
                //SDL_FreeSurface(sizeSurf);
            }
        }
    }
	printf("sortie cutgrid\n");
    SDL_FreeSurface(originImg);
}

//Remove inconsistent lines
int RemoveNoise(struct Line* list, size_t* length, size_t nbToRemove){

    Line_sort(list, 0, *length);

    size_t removed = 0;

    while(removed < nbToRemove){

        size_t* forward = calloc(*length-1, sizeof(size_t));
        size_t* fwdAndBcw = calloc(*length, sizeof(size_t));
        size_t* farFromAverage = calloc(*length, sizeof(size_t));

        for (size_t i = 0; i < *length-1; i++)
        {
            forward[i] = abs(abs(list[i+1].rho) - abs(list[i].rho));
        }

        for (size_t i = 0; i < *length; i++)
        {
            size_t sum = 0;
            size_t nb = 0;
            if(i > 0){
                sum += forward[i-1];
                nb++;
            }
            if(i < *length - 1){
                sum += forward[i];
                nb++;
            }
            fwdAndBcw[i] = sum/nb;
        }
        
        int average = AverageOfList(fwdAndBcw, *length);

        for (size_t i = 0; i < *length; i++)
        {
            farFromAverage[i] = abs(average - (int)fwdAndBcw[i]);
        }

        //print lists
        /*printf("List (sorted) = ");
        for (size_t i = 0; i < *length; i++)
        {
            printf("%4i |", list[i].rho);
        }
        printf("\n");

        printf("forwardGap    = ");
        for (size_t i = 0; i < *length - 1; i++)
        {
            printf("%4zu |", forward[i]);
        }
        printf("\n");

        printf("Bck&FwdGap(%i)= ", average);
        for (size_t i = 0; i < *length; i++)
        {
            printf("%4zu |", fwdAndBcw[i]);
        }
        printf("\n");

        printf("farFromAvg(%i)= ", average);
        for (size_t i = 0; i < *length; i++)
        {
            printf("%4zu |", farFromAverage[i]);
        }
        printf("\n");*/


        size_t ind_max = maxList_(farFromAverage, 0, *length);
        //printf("Removing: %zueme line rho = %i\n", ind_max, list[ind_max].rho);
        list_pop(list, *length, ind_max);
        *length -= 1;
        removed++;
        
        
        free(fwdAndBcw);
        free(forward);
        free(farFromAverage);

    }

    return removed;
}

//Main fonction which complete segmentation 
int Segmentation(SDL_Surface* originalImage, SDL_Surface* sobelImage,
 char* linesImgPath, int case_coor[81][2], SDL_Surface boxesArray[81]){

    if(SDL_LockSurface(sobelImage) != 0)
        printf("Unable to lock the surface");
	IMG_SaveJPG(sobelImage, "./sobelImage.jpg", 100);
	IMG_SaveJPG(originalImage, "./originImage.jpg", 100);

    int W = sobelImage->w;//Width of the edge picture
    int H = sobelImage->h;//Height of the edge picture
   
    int maxRho = diagLen(W,H); //rho values : [-maxRho, maxRho]
    int maxTheta = 180; //theta values : [0, maxTheta]
    int nbLines = 24; //number of lines to print
    

    //Create and Fill the hough accumulator
    int* Acc = HoughAccu(sobelImage->pixels, W, H, maxRho, maxTheta);


    size_t H_len = nbLines;
    struct Line* H_lines = calloc(H_len, sizeof(struct Line));
    size_t V_len = nbLines;
    struct Line* V_lines = calloc(V_len, sizeof(struct Line));


    //Compute and print the lines
    int AngleToRotate = ComputeLines(nbLines, Acc, maxRho, maxTheta,
     H_lines, &H_len, V_lines, &V_len);
    printf("Angle To Rotate: %i\n\n", AngleToRotate);
    if (abs(AngleToRotate) > 5 /*&& !notRotated*/)
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

    //Draw detected lines
    PrintLines(sobelImage, H_lines, H_len, W, H);
    PrintLines(sobelImage, V_lines, V_len, W, H);
    //Saving picture with drawn lines
    IMG_SaveJPG(sobelImage, linesImgPath, 100);
    //SDL_FreeSurface(sobelImage);

    
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
//	char filename_[] = "";
    
    CutGrid(originalImage, intersXY, filename_, case_coor, boxesArray, 10, 11);
	printf("bug3\n");

    
    //Cleaning
    free(H_lines);
    free(V_lines);
    free(Acc);
    free(intersXY);

    return EXIT_SUCCESS;
}
