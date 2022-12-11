#include <math.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

///// Rajoute par charles pour ubuntu
# define M_PI           3.14159265358979323846  /* pi */
//////
//
Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	switch (bpp)
	{
	case 1:
		return *p;
	case 2:
		return *(Uint16 *)p;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
	case 4:
		return *(Uint32 *)p;
	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}

void inline set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else
		{
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:

		*(Uint32 *)p = pixel;
		break;
	}
}

// central rotation, automatically allocate memory
SDL_Surface *SDL_RotationCentral(SDL_Surface *origine, float angle)
{
	SDL_Surface *destination;
	int i;
	int j;
	Uint32 color;
	int mx, my, mxdest, mydest;
	int bx, by;
	float angle_radian;
	float tcos;
	float tsin;
	double widthdest;
	double heightdest;
	angle_radian = -angle * M_PI / 180.0;	// angle in radian
	tcos = cos(angle_radian);
	tsin = sin(angle_radian);
	widthdest = ceil(origine->w * fabs(tcos) + origine->h * fabs(tsin)),
	heightdest = ceil(origine->w * fabs(tsin) + origine->h * fabs(tcos)),
	destination = SDL_CreateRGBSurface(SDL_SWSURFACE, widthdest, heightdest, origine->format->BitsPerPixel,
										origine->format->Rmask, origine->format->Gmask, origine->format->Bmask, origine->format->Amask);
	if (destination == NULL)
		return NULL;

	mxdest = destination->w / 2.;	// center of destination
	mydest = destination->h / 2.;	// center of destination
	mx = origine->w / 2.;		// center of origine
	my = origine->h / 2.;	// center of origine

	for (j = 0; j < destination->h; j++)
		for (i = 0; i < destination->w; i++)
		{
			/* on détermine la valeur de pixel qui correspond le mieux pour la position
			 * i,j de la surface de destination */

			/* on détermine la meilleure position sur la surface d'origine en appliquant
			 * une matrice de rotation inverse
			 */

			bx = (ceil(tcos * (i - mxdest) + tsin * (j - mydest) + mx));
			by = (ceil(-tsin * (i - mxdest) + tcos * (j - mydest) + my));
			/* on vérifie que l'on ne sort pas des bords*/
			if (bx >= 0 && bx < origine->w && by >= 0 && by < origine->h)
			{
				color = get_pixel(origine, bx, by);
				set_pixel(destination, i, j, color);
			}
		}
	//SDL_SaveBMP(destination, "image_rotate.bmp");	// save image
	return destination;
}
