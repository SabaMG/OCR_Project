#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>
#include <SDL2/SDL.h>

size_t maxList(int _list[], size_t len);
size_t minList(size_t _list[], size_t len);
int diagLen(int w, int h);
void HoughAngle(SDL_Surface* img, int Hough_lines[][91], size_t w_);
size_t MaxIndex2DArray(int table[][91], size_t w, size_t h);
void PixelOnColsAndLines(Uint32* pixels, int linesX[], int linesY[],
 size_t lenX, size_t lenY);
void Sort10MaxsIndexs(int list[], size_t len, size_t dest[]);
void DisplayLines(Uint32* pixels, size_t listX[], size_t listY[],
 size_t ImgW, size_t ImgH);
void CutAndSaveBoxes(char* PictPath, size_t listOfX[], size_t listOfY[],
 char* pathToSave, size_t iIndex, size_t jIndex);
void maxInAccu(int* Accu, int nbRhos, int nbThetas, int* _rho, int* _theta);
Uint32* accToPic(int* Acc, int* w, int* h);
void BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w, int h);
int* HoughAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta);
Uint32* listToPic(int* listIndex, int length, int w);
int PrintLines(SDL_Surface* img, int nbLines, int* Acc, int maxRho, int maxTheta, int* H_lines, int* V_lines);
int GetIntersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int* X, int* Y);
struct Point* ComputeInters(int* H_lines, size_t H_len, int* V_lines, size_t V_len, int nbLines);
void CutGrid(char* imgPath, struct Point* inters, char* pathToSave,
 size_t iIndex, size_t jIndex, size_t nbLines);


struct Point
{
    int x;
    int y;
};

#endif
