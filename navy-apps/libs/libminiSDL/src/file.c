#include <sdl-file.h>
#include <stdlib.h>
#include <SDL.h>

int64_t __sdl_size(SDL_RWops *f) {
	fseek(f->fp, 0, SEEK_END);
	int size = ftell(f->fp);
	fseek(f->fp, 0, SEEK_SET);
	return size;
}

int64_t __sdl_seek(SDL_RWops *f, int64_t offset, int whence) {
	fseek(f->fp, offset, whence);
	return ftell(f->fp);
}

size_t __sdl_read(SDL_RWops *f, void *buf, size_t size, size_t nmemb) {
	return fread(buf, size, nmemb, f->fp);
}

size_t __sdl_write(SDL_RWops *f, const void *buf, size_t size, size_t nmemb) {
	return fwrite(buf, size, nmemb, f->fp);
}

int __sdl_close(SDL_RWops *f) {
	return fclose(f->fp);
}

SDL_RWops *SDL_RWFromFile(const char *filename, const char *mode) {
	SDL_RWops *res = (SDL_RWops *)malloc(sizeof(SDL_RWops));
	res->type = RW_TYPE_FILE;
	res->fp = fopen(filename, mode);
	res->size = __sdl_size;
	res->seek = __sdl_seek;
	res->read = __sdl_read;
	res->write = __sdl_write;
	res->close = __sdl_close;
	return res;
}

SDL_RWops *SDL_RWFromMem(void *mem, int size) {
	SDL_RWops *res = (SDL_RWops *)malloc(sizeof(SDL_RWops));
	res->type = RW_TYPE_MEM;
	res->mem.base = mem;
	res->mem.size = size;
	res->fp = fmemopen(mem, size, "r+");
	res->size = __sdl_size;
	res->seek = __sdl_seek;
	res->read = __sdl_read;
	res->write = __sdl_write;
	res->close = __sdl_close;
	return res;
}
