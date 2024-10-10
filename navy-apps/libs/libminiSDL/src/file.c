#include <sdl-file.h>
#include <stdlib.h>
#include <SDL.h>

SDL_RWops *SDL_RWFromFile(const char *filename, const char *mode) {
	SDL_RWops *res = (SDL_RWops *)malloc(sizeof(SDL_RWops));
	res->type = RW_TYPE_FILE;
	return NULL;
}

SDL_RWops *SDL_RWFromMem(void *mem, int size) {
	SDL_RWops *res = (SDL_RWops *)malloc(sizeof(SDL_RWops));
	res->type = RW_TYPE_MEM;
	return NULL;
}
