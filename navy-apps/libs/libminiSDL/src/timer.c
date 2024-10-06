#include <NDL.h>
#include <SDL.h>
#include <sdl-timer.h>
#include <stdio.h>

#define MAX_EVT_NR 16

extern int sdl_start_time;
static TimerEvent events[MAX_EVT_NR];
static int event_count = 0;

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
	events[event_count].interval = interval;
	events[event_count].callback = callback;
	events[event_count].param = param;
	event_count++;
	return events + event_count - 1;
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

void SDL_SystemTimerHandle(int signum) {
	static uint64_t local_time = 0;
	local_time += MACH_HZ;
	for (int i = 0; i < event_count; i++) {
		if (events[i].callback && local_time % events[i].interval == 0) {
			events[i].callback(events[i].interval, events[i].param);
		}
	}
}
