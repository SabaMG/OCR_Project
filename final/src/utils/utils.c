#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface *copy_surface(SDL_Surface *source) {
    SDL_Surface* copie = SDL_CreateRGBSurface(0, source->w, source->h, 32,0,0,0,0);
    SDL_SetSurfaceBlendMode(source, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(source, NULL, copie, NULL);
    return copie;
}

SDL_Surface* sdl_surface_new_from_gdk_pixbuf(SDL_Surface* source, GdkPixbuf *pixbuf) {
    int rowstride;
    guchar *pixels;

    pixels = gdk_pixbuf_get_pixels (pixbuf);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);

    Uint32 src_format = source->format->format;
    Uint32 dst_format = SDL_PIXELFORMAT_RGB24;

    SDL_Surface* surface = SDL_CreateRGBSurface(0, source->w, source->h, 32,0,0,0,0);
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, src_format,
               surface->pixels, surface->pitch,
               dst_format, pixels, rowstride);
    SDL_UnlockSurface(surface);
	return surface;
}

GdkPixbuf * gtk_image_new_from_sdl_surface(SDL_Surface *surface) {
    Uint32 src_format;
    Uint32 dst_format;

    GdkPixbuf *pixbuf;
    gboolean has_alpha;
    int rowstride;
    guchar *pixels;

    // select format                                                            
    src_format = surface->format->format;
    has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);
    if (has_alpha) {
        dst_format = SDL_PIXELFORMAT_RGBA32;
    }
    else {
        dst_format = SDL_PIXELFORMAT_RGB24;
    }

    // create pixbuf                                                            
    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8,
                             surface->w, surface->h);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels (pixbuf);

    // copy pixels                                                              
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, src_format,
               surface->pixels, surface->pitch,
               dst_format, pixels, rowstride);
    SDL_UnlockSurface(surface);

	return pixbuf;
}

// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format) {
	Uint8 r, g, b;
	// Get r, g, b from the pixel.
	SDL_GetRGB(pixel_color, format, &r, &g, &b);
	// Determine average.
	Uint8 average = 255 - (0.3*r + 0.59*g + 0.11*b);
	// Set r, g, b to average.
	r = average;
	g = average;
	b = average;
	// Return new color.
	return SDL_MapRGB(format, r, g, b);
}

void surface_to_grayscale(SDL_Surface* surface) {
	// Get array of pixels.
	Uint32* pixels = surface->pixels;
	// Get len of the array.
	int len = surface->w * surface->h;
	// Get the format of the surface.
	SDL_PixelFormat* format = surface->format;
	// Lock the surface.
	if (SDL_LockSurface(surface) != 0)
		printf("error\n");
		//errx(EXIT_FAILURE, "%s", SDL_GetError());
	// Convert each pixel to gray.
	for (int i = 0; i < len; i++) {
		pixels[i] = pixel_to_grayscale(pixels[i], format);
	}
	// Unlock the surface.
	SDL_UnlockSurface(surface);
}
