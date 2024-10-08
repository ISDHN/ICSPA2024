#include <NDL.h>
#include <SDL.h>
#include <sys/signal.h>

int sdl_start_time = 0;
extern void (*SDL_SystemTimerHandle)(int);

int SDL_Init(uint32_t flags) {
	sdl_start_time = NDL_GetTicks();
	signal(SIGALRM, SDL_SystemTimerHandle);
	return NDL_Init(flags);
}

void SDL_Quit() {
	NDL_Quit();
}

char *SDL_GetError() {
	return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char *fmt, ...) {
	TODO()

	return -1;
}

int SDL_ShowCursor(int toggle) {
	TODO()

	return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
	// TODO()
}
