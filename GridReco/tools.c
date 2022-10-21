#include "tool.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

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

size_t diagLen(size_t w, size_t h){
    return (size_t)(sqrt((w * w) + (h * h)));
}
