#include <NDL.h>
#include <SDL.h>
#include <sys/signal.h>

int sdl_start_time = 0;
void SDL_SystemTimerHandle(int);
_sig_func_ptr _signal(int signum, _sig_func_ptr handler);

int SDL_Init(uint32_t flags) {
	sdl_start_time = NDL_GetTicks();
	_signal(SIGALRM, SDL_SystemTimerHandle);
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
