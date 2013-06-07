
#include "bitmap.h"

#include <SDL/SDL.h>

struct bitmap
{
	int width, height;
	int bitsPerPixel;
	int bytesPerPixel;
	void *pixels;
};

BITMAP bitmap_read(const char *filename)
{
	SDL_Surface *original = SDL_LoadBMP(filename);
	if(original == NULL) {
		printf("Error: bitmap file at [%s] could not be opened.\n", filename);
		return NULL;
	}
	BITMAP image = (BITMAP)malloc(sizeof(struct bitmap));
	if(image == NULL) {
		SDL_FreeSurface(original);
		return NULL;
	}

	SDL_Surface *surface = original;//SDL_DisplayFormatAlpha(original);
//	SDL_FreeSurface(original);

	image->width = surface->w;
	image->height = surface->h;
	image->bitsPerPixel = surface->format->BitsPerPixel;
	image->bytesPerPixel = surface->format->BytesPerPixel;

	int bytesPerPixel = image->bytesPerPixel;
	if(bytesPerPixel == 3) bytesPerPixel = 4;

	int size = image->width * image->height * bytesPerPixel;
	image->pixels = (void *)malloc(size);
	if(image->pixels == NULL) {
		SDL_FreeSurface(surface);
		free(image);
		return NULL;
	}

	SDL_LockSurface(surface);
	if(image->bytesPerPixel == 3) {
		image->bytesPerPixel = 4;
		image->bitsPerPixel = 32;

		unsigned char *data = (unsigned char *)image->pixels;
		int i, j = 0;
		for(i = 0; i < size; ++i)
		{
			if((i+1) % 4 == 0) {
				float intensity = (data[i-1] + data[i-2] + data[i-3]) / 3.0f;
				if(intensity < 20)
					intensity = 0;
				else
					intensity = 255;
				data[i] = (unsigned char)intensity;
	        } else {
	        	data[i] = ((unsigned char *)surface->pixels)[j++];
	        }
	    }
	} else {
		printf("Info: bitmap file at [%s] at [%d]\n", filename, image->bitsPerPixel);
		memcpy(image->pixels, surface->pixels, size);
	}
	SDL_UnlockSurface(surface);

	SDL_FreeSurface(surface);
	return image;
}
void bitmap_free(BITMAP image)
{
	if(image == NULL) return;
	free(image->pixels);
	free(image);
}

int bitmap_width(BITMAP image)
{
	if(image == NULL) return -1;
	return image->width;
}
int bitmap_height(BITMAP image)
{
	if(image == NULL) return -1;
	return image->height;
}

int bitmap_bitsPerPixel(BITMAP image)
{
	if(image == NULL) return -1;
	return image->bitsPerPixel;
}
int bitmap_bytesPerPixel(BITMAP image)
{
	if(image == NULL) return -1;
	return image->bytesPerPixel;
}

void *bitmap_pixels(BITMAP image)
{
	if(image == NULL) return NULL;
	return image->pixels;
}