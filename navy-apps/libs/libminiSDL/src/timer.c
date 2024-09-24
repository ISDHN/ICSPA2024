#include <NDL.h>
#include <SDL.h>
#include <sdl-timer.h>
#include <stdio.h>

extern int sdl_start_time;

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
	TODO()

	return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
	TODO()

	return 1;
}

uint32_t SDL_GetTicks() {
	return NDL_GetTicks() - sdl_start_time;
}

void SDL_Delay(uint32_t ms) {
	int start = NDL_GetTicks();
	while (NDL_GetTicks() - start < ms)
		;
}
