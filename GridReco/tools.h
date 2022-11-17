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
int* houghtransform(Uint32 *pixels, int *w, int *h, int* _rho, int* _theta);
int* houghAccumulator(Uint32* pixels, int* w, int* h, int* _nbRhos, int* _nbThetas);
void maxInAccu(int* Accu, int* nbRhos, int* nbThetas, int* _rho, int* _theta);
void displayOneLine(Uint32* pixels, int rho, int theta, int* w, int* h);
Uint32* accToPic(int* Acc, int* w, int* h);
int* AnotherAccu(Uint32* pixels, int* w, int* h);
void BresenhamLine(Uint32* pixels, int x1, int y1, int x2, int y2, int w, int h);
int* FinalAccu(Uint32* pixels, int W, int H, int maxRho, int maxTheta);
Uint32* listToPic(int* listIndex, int length, int w);
