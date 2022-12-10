#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>
#include <SDL2/SDL.h>

struct Point
{
    int x;
    int y;
};

struct Line
{
    int rho;
    int theta;
    int x1;
    int y1;
    int x2;
    int y2;
};


int diagLen(int w, int h);

void maxInAccu(int* Accu, int nbRhos, int nbThetas, int* _rho, int* _theta);

void BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w,
 int h);
int* HoughAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta);

int ComputeLines(int nbLines, int* Acc, int maxRho,
 int maxTheta, struct Line* H_lines, size_t* H_len, struct Line* V_lines,
  size_t* V_len);
int GetIntersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4,
 int y4, int* X, int* Y);
struct Point* ComputeInters(struct Line* H_lines, struct Line* V_lines);
void CutGrid(SDL_Surface* originImg, struct Point* inters, char* pathToSave,
 int case_coor[81][2], SDL_Surface boxesArray[81], size_t iIndex,
  size_t jIndex);

int RemoveNoise(struct Line* list, size_t* length, size_t nbToRemove);

void PrintLines(SDL_Surface* img, struct Line* list, size_t length, int img_w,
 int img_h);
//Main fonction which complete segmentation 
int Segmentation(SDL_Surface* originalImage, SDL_Surface* sobelImage,
 char* linesImgPath, int case_coor[81][2], SDL_Surface boxesArray[81]);


#endif
