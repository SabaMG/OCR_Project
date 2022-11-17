#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>
#include <SDL2/SDL.h>

size_t maxList(int _list[], size_t len);
size_t minList(size_t _list[], size_t len);
size_t diagLen(size_t w, size_t h);
void HoughAngle(SDL_Surface* img, int Hough_lines[][91], size_t w_);
size_t MaxIndex2DArray(int table[][91], size_t w, size_t h);
void PixelOnColsAndLines(Uint32* pixels, int linesX[], int linesY[],
 size_t lenX, size_t lenY);
void Sort10MaxsIndexs(int list[], size_t len, size_t dest[]);
void DisplayLines(Uint32* pixels, size_t listX[], size_t listY[],
 size_t ImgW, size_t ImgH);
void CutAndSaveBoxes(char* PictPath, size_t listOfX[], size_t listOfY[],
 char* pathToSave, size_t iIndex, size_t jIndex);
void resize_boxe(SDL_Rect* case_);
void remove_lines(SDL_Rect* case_);
void setPixel32(SDL_Surface* surface, int x, int y, Uint32 color);
Uint32 getPixel(SDL_Surface* surface, int x, int y);
SDL_Surface* resize(SDL_Surface* source, float zoom);
void display(char* name, SDL_Surface* surf);
int min(int x, int y);

#endif
