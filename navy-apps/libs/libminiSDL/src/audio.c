#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <sdl-audio.h>
#include <wav.h>

SDL_AudioSpec spec;

bool is_pause = true;
SDL_AudioCallback callback;

void audio_callback_caller() {
	// if (is_pause) {
	// 	return;
	// }
	// static int last_time = 0;
	// int now = NDL_GetTicks();
	// if (now - last_time < spec.samples * 256 / spec.freq) {
	// 	return;
	// }
	// last_time = now;
	// uint8_t *audio_buf = malloc(spec.samples);
	// callback(NULL, audio_buf, spec.samples);
	// NDL_PlayAudio(audio_buf, spec.samples);
	// free(audio_buf);
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
	// assert(desired != NULL);
	// spec = *desired;
	// callback = desired->callback;
	// NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
	// if (obtained != NULL) {
	// 	*obtained = *desired;
	// }
	return 0;
}

void SDL_CloseAudio() {
	// NDL_CloseAudio();
}

void SDL_PauseAudio(int pause_on) {
	is_pause = pause_on;
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
	// for (int i = 0; i < len; i++) {
	// 	dst[i] = (dst[i] * (SDL_MIX_MAXVOLUME - volume) + src[i] * volume) / SDL_MIX_MAXVOLUME;
	// }
}

inline bool CheckWav(wav_info *info) {
	return info->riff[0] == 'R' && info->riff[1] == 'I' && info->riff[2] == 'F' && info->riff[3] == 'F' &&
		   info->wave[0] == 'W' && info->wave[1] == 'A' && info->wave[2] == 'V' && info->wave[3] == 'E' &&
		   info->fmt[0] == 'f' && info->fmt[1] == 'm' && info->fmt[2] == 't' && info->fmt[3] == ' ' &&
		   info->data[0] == 'd' && info->data[1] == 'a' && info->data[2] == 't' && info->data[3] == 'a' &&
		   info->fmt_size == 16 && info->format == 1;
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
	FILE *fp = fopen(file, "r");
	assert(fp);
	wav_info info;
	fread(&info, 1, sizeof(wav_info), fp);
	if (!CheckWav(&info)) {
		fclose(fp);
		return NULL;
	}
	*audio_buf = malloc(info.data_size);
	spec->freq = info.sample_rate;
	spec->channels = info.channels;
	fread(*audio_buf, 1, info.data_size, fp);
	spec->size = info.data_size;
	*audio_len = info.data_size;
	fclose(fp);
	return spec;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
	free(audio_buf);
}

void SDL_LockAudio() {
	// TODO()
}

void SDL_UnlockAudio() {
	// TODO()
}
