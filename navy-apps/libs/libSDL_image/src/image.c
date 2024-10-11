#define SDL_malloc malloc
#define SDL_free free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"
#include <stdio.h>

SDL_Surface *IMG_Load_RW(SDL_RWops *src, int freesrc) {
	assert(src->type == RW_TYPE_MEM);
	assert(freesrc == 0);
	long size = src->size(src);
	char *buffer = (char *)malloc(size);
	src->read(src, buffer, 1, size);
	SDL_Surface *image = STBIMG_LoadFromMemory(buffer, size);
	free(buffer);
	return image;
}

SDL_Surface *IMG_Load(const char *filename) {
	FILE *img = fopen(filename, "rb");
	assert(img);
	fseek(img, 0, SEEK_END);
	long size = ftell(img);
	char *buffer = (char *)malloc(size);
	fseek(img, 0, SEEK_SET);
	fread(buffer, 1, size, img);
	SDL_Surface *image = STBIMG_LoadFromMemory(buffer, size);
	free(buffer);
	fclose(img);
	return image;
}

int IMG_isPNG(SDL_RWops *src) {
	char buff[8];
	char *head = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
	src->read(src, buff, 1, 8);
	return !strcmp(buff, head);
}

SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src) {
	return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
	return "Navy does not support IMG_GetError()";
}
