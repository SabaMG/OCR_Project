#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_rotozoom.h>

void Rotation_Manuel_F(SDL_Surface *image, double angle)
{
    IMG_SaveJPG(rotozoomSurface(image, angle, 1.0, 2), "rotated_image.jpg", 100);
    SDL_FreeSurface(image);
}

unsigned int str_to_uint(char s[])
{
        unsigned int res = s[0] -'0';
        size_t i = 1;

        while(s[i] != 0)
        {
        	res = res * 10 + s[i] -'0';
        	i++;
        }

    	return res;
}
