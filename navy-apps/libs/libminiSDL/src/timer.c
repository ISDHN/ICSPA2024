#include <NDL.h>
#include <SDL.h>
#include <sdl-timer.h>
#include <stdio.h>
#include <string.h>

#define MAX_EVT_NR 16

extern int sdl_start_time;
static TimerEvent events[MAX_EVT_NR];

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
	for (int i = 0; i < MAX_EVT_NR; i++) {
		if (events[i].callback) {
			continue;
		}
		events[i].interval = interval;
		events[i].callback = callback;
		events[i].param = param;
		return events + i;
	}
	return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
	memset(id, 0, sizeof(TimerEvent));
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

void SDL_SystemTimerHandle(int signum) {
	for (int i = 0; i < MAX_EVT_NR; i++) {
		if (events[i].callback) {
			events[i].local += MACH_HZ;
			if (events[i].local >= events[i].interval) {
				events[i].local %= events[i].interval;
				events[i].callback(events[i].interval, events[i].param);
			}
		}
	}
}
