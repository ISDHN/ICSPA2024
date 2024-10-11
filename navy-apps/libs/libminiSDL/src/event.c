#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define key(k) [SDLK_##k] = #k "\n", // there in \n in the original keycode
#define init_keystate(k) [SDLK_##k] = 0,
#define ACT_IND 1
#define KEY_IND 3
static const char *keyname[] = {
	"NONE",
	_KEYS(key)}; // there in \n in the original code

static uint8_t key_state[] = {
	_KEYS(init_keystate)};

#define EVT_QUEUE_LEN 1024
static SDL_Event evt_queue[EVT_QUEUE_LEN] = {};
static int evt_l = 0, evt_r = 0;

static int evt_enqueue(SDL_Event *evt) {
	if (evt_r == (evt_l - 1 + EVT_QUEUE_LEN) % EVT_QUEUE_LEN) {
		printf("event overflow\n");
		return -1;
	}
	evt_queue[evt_r] = *evt;
	evt_r = (evt_r + 1) % EVT_QUEUE_LEN;
	return 0;
}

int SDL_PushEvent(SDL_Event *ev) {
	return evt_enqueue(ev);
}

static void Classify_Keyboard_Event(const char *buf, SDL_Event *event) {
	event->type = buf[ACT_IND] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
	for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++) {
		if (strcmp(keyname[i], buf + KEY_IND) == 0) {
			event->key.keysym.sym = i;
			break;
		}
	}
}

int SDL_PollEvent(SDL_Event *ev) {
	char buf[32] = {0};
	SDL_Event local_evt_buf;
	while (NDL_PollEvent(buf, sizeof(buf))) {
		Classify_Keyboard_Event(buf, &local_evt_buf);
		SDL_PushEvent(&local_evt_buf);
		key_state[local_evt_buf.key.keysym.sym] = local_evt_buf.type == SDL_KEYDOWN ? 1 : 0;
		memset(buf, 0, 32);
	}
	if (evt_l != evt_r) {
		if (ev) {
			*ev = evt_queue[evt_l];
			evt_l = (evt_l + 1) % EVT_QUEUE_LEN;
		}
		return 1;
	} else {
		return 0;
	}
}

int SDL_WaitEvent(SDL_Event *event) {
	while (!SDL_PollEvent(event)) {
	}
	return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
	assert(action == SDL_GETEVENT);
	assert(numevents == 1);
	for (int i = evt_l; i != evt_r; i = (i + 1) % EVT_QUEUE_LEN) {
		if (evt_queue[i].type & mask) {
			*ev = evt_queue[i];
			for (int j = i; j != evt_r; j = (j + 1) % EVT_QUEUE_LEN) {
				evt_queue[j] = evt_queue[(j + 1) % EVT_QUEUE_LEN];
			}
			evt_r = (evt_r - 1 + EVT_QUEUE_LEN) % EVT_QUEUE_LEN;
			return 1;
		}
	}
	return -1;
}

uint8_t *SDL_GetKeyState(int *numkeys) {
	SDL_Event event;
	while (SDL_PollEvent(&event))
		;
	return key_state;
}
