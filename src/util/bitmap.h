#pragma once

typedef struct bitmap *BITMAP;

#ifdef __cplusplus
extern "C" {
#endif

BITMAP bitmap_read(const char *filename);
void bitmap_free(BITMAP image);

int bitmap_width(BITMAP image);
int bitmap_height(BITMAP image);

int bitmap_bitsPerPixel(BITMAP image);
int bitmap_bytesPerPixel(BITMAP image);

void *bitmap_pixels(BITMAP image);

#ifdef __cplusplus
}
#endif