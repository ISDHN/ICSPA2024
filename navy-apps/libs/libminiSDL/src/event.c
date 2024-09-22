#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define key(k) [SDLK_##k] = #k "\n", // there in \n in the original code

#define ACT_IND 1
#define KEY_IND 3

static const char *keyname[] = {
	"NONE",
	_KEYS(key)}; // there in \n in the original code

int SDL_PushEvent(SDL_Event *ev) {
	TODO()

	return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
	TODO()

	return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	char buf[32] = {0};
	while (!NDL_PollEvent(buf, sizeof(buf)))
		;
	event->type = buf[ACT_IND] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
	for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++) {
		if (strcmp(keyname[i], buf + KEY_IND) == 0) {
			event->key.keysym.sym = i;
			break;
		}
	}
	return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
	TODO()

	return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys) {
	TODO()

	return NULL;
}
