#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <sdl-audio.h>

SDL_AudioSpec spec;

bool is_pause = true;
SDL_AudioCallback callback;

void audio_callback_caller() {
	static int last_time = 0;
	if (is_pause) {
		return;
	}
	int now = NDL_GetTicks();
	if (now - last_time < spec.samples / spec.freq) {
		return;
	}
	last_time = now;
	uint8_t *audio_buf = malloc(spec.samples);
	callback(NULL, audio_buf, spec.samples);
	NDL_PlayAudio(audio_buf, spec.samples);
	free(audio_buf);
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
	assert(desired != NULL);
	spec = *desired;
	callback = desired->callback;
	NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
	if (obtained != NULL) {
		*obtained = *desired;
	}
	return 0;
}

void SDL_CloseAudio() {
	NDL_CloseAudio();
}

void SDL_PauseAudio(int pause_on) {
	is_pause = pause_on;
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
	// TODO()
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
	// TODO()

	return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
	// TODO()
}

void SDL_LockAudio() {
	// TODO()
}

void SDL_UnlockAudio() {
	// TODO()
}
