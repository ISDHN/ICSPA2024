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

static void Classify_Keyboard_Event(const char *buf, SDL_Event *event) {

	audio_callback_caller();
	event->type = buf[ACT_IND] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;

	audio_callback_caller();
	for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++) {

		audio_callback_caller();
		if (strcmp(keyname[i], buf + KEY_IND) == 0) {

			audio_callback_caller();
			event->key.keysym.sym = i;

			audio_callback_caller();
			break;
		}
	}
}

int SDL_PollEvent(SDL_Event *ev) {
	audio_callback_caller();
	char buf[32] = {0};

	audio_callback_caller();
	int res = NDL_PollEvent(buf, sizeof(buf));

	audio_callback_caller();
	if (res != 0) {

		audio_callback_caller();
		Classify_Keyboard_Event(buf, ev);

		audio_callback_caller();
		return 1;
	}

	audio_callback_caller();
	return 0;
}

int SDL_WaitEvent(SDL_Event *event) {

	audio_callback_caller();
	char buf[32] = {0};

	audio_callback_caller();
	while (!NDL_PollEvent(buf, sizeof(buf))) {
		audio_callback_caller();
	}

	audio_callback_caller();
	Classify_Keyboard_Event(buf, event);
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
