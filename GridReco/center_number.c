#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "center_number.h"
#include "flood_fill.h"

// This function ajuste the coordinate of the case for center the number
void ajuste_case(SDL_Surface* source, SDL_Rect* case_)
{
    // remove lines of grid
    remove_lines(case_);

    // Get center coordinate of number in case
    int centerX = 0, centerY = 0;
    SDL_Surface* centerSurf = SDL_CreateRGBSurface(0, case_->w, case_->h, 32, 0, 0, 0, 0);
    SDL_UnlockSurface(centerSurf);
    if(SDL_BlitSurface(source, case_, centerSurf, NULL) == 0)
    {
        center_pixel(centerSurf, &centerX, &centerY, 3.5);
    }

    // center number in the case
    int deltaX = 0;
    int deltaY = 0;
    if(centerX != 0 && centerY != 0)
    {
        deltaX = centerX - case_->w / 2;
        deltaY = centerY - case_->h / 2;
    }
    case_->x += deltaX;
    case_->y += deltaY;

    // free surface
    SDL_FreeSurface(centerSurf);
}

// This function store coordinate of the center of number
// in x and y vriables
void center_pixel(SDL_Surface* s, int* x, int* y, float seuil)
{
    Uint32* pixels = s->pixels;

    // Define the grey treshold for pixels
    int av_surface = average_surface(s);
    av_surface -= (int)((float)av_surface / seuil);

    // Init values for store coordinate pixels
    int min_coorX = s->w, max_coorX = 0;
    int min_coorY = s->h, max_coorY = 0;
    int nb_pixel = 0;
    
    // Save X coordinate of pixel (grey average)
    for(int i = 0; i < s->h; ++i)
    {
        for(int j = 0; j < s->w; ++j)
        {
            int av_pixel = average_pixel(pixels[i * s->h + j], s->format);
            if(av_pixel < av_surface)
            {
                if(j < min_coorX)
                {
                    min_coorX = j;
                }
                else if(j > max_coorX)
                {
                    max_coorX = j;
                }
                
                if(i < min_coorY)
                {
                    min_coorY = i;
                }
                else if(i > max_coorY)
                {
                    max_coorY = i;
                }
                nb_pixel += 1;
            }
        }
    }

    // save the coordinate of the pixel
    if(nb_pixel > 5)
    {
        *x = (min_coorX + max_coorX) / 2;
        *y = (min_coorY + max_coorY) / 2;
    }
}

// This function calculate the grey level of a pixel
int average_pixel(Uint32 pixel, SDL_PixelFormat* format)
{
    // Store rgg coordinate of pixel
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    
    // Calculate the grey level of pixel
    return (r + g + b) / 3;
}

// This function calculate the average of grey in the surface s
int average_surface(SDL_Surface* s)
{
    Uint32* pixels = s->pixels;
    int av_pixel = 0;
    int len = s->h * s->w;

    for(int j = 0; j < len; ++j)
    {
        av_pixel += average_pixel(pixels[j], s->format);
    }

    return av_pixel / len;
}

// This function cut the lines of sudoku
void remove_lines(SDL_Rect* case_)
{
    case_->x += case_->w / 5;
    case_->y += case_->h / 5;
    case_->w -= (2 * case_->w) / 5;
    case_->h -= (2 * case_->h) / 5;
}

/// Those functions are use for make a zoom of the surface, 
///
void setPixel32(SDL_Surface* surface, int x, int y, Uint32 couleur) 
{
    Uint32 *p = surface->pixels + (y) * surface->pitch + (x) * 4;
    *p = couleur;
}

Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
    Uint32 *p = (Uint32 *)(surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
    return *p;   
}

SDL_Surface* resize(SDL_Surface *source, float zoom)
{
    int hauteur = (int)(source->h * zoom);
    int largeur = (int)(source->w * zoom);

    SDL_Surface *laNew = SDL_CreateRGBSurface(0, largeur, hauteur, 32, 0, 0, 0, 0);
    for(int y = 0; y < hauteur; y++)
    {
        for(int x = 0; x < largeur; x++)
        {
            setPixel32(laNew, x, y, getPixel(source, (int)(x / zoom), (int)(y / zoom)));
        }
    }

    return laNew;
}
///
///
