/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <SDL2/SDL.h>
#include <common.h>
#include <device/map.h>

#define TEMP_BUF_SIZE 1
enum {
	reg_freq,
	reg_channels,
	reg_samples,
	reg_sbuf_size,
	reg_init,
	reg_count,
	nr_reg
};

static uint8_t *temp_buf = NULL;
static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static uint32_t pos_l = 0, pos_r = 0;

static inline int get_count() {
	return pos_r - pos_l + (pos_r > pos_l) ? CONFIG_SB_SIZE : 0;
}

static void callback(void *userdata, uint8_t *stream, int len) {
	int count = get_count();
	Log("count = %d", count);
	if (count > len) {
		count = len;
	}
	for (int i = 0; i < count; i++) {
		stream[i] = sbuf[pos_l];
		pos_l = (pos_l + 1) % CONFIG_SB_SIZE;
	}
	memset(stream + count, 0, len - count);
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
	if (is_write) {
		if (audio_base[reg_init] == 1) {
			SDL_AudioSpec spec;
			spec.freq = audio_base[reg_freq];
			spec.format = AUDIO_S16SYS;
			spec.channels = audio_base[reg_channels];
			spec.samples = audio_base[reg_samples];
			spec.callback = callback;
			spec.userdata = NULL;
			SDL_InitSubSystem(SDL_INIT_AUDIO);
			SDL_OpenAudio(&spec, NULL);
			SDL_PauseAudio(0);
		}
	} else {
		audio_base[reg_count] = get_count();
	}
}

static void audio_buffer_handle(uint32_t offset, int len, bool is_write) {
	assert(len == 1);
	assert(offset == 0);
	if (is_write) {
		sbuf[pos_r] = temp_buf[0];
		pos_r = (pos_r + 1) % CONFIG_SB_SIZE;
#ifdef CONFIG_WARN_OVERFLOW
		if (pos_r == pos_l) {
			Warning("audio buffer  overflow");
		}
#endif
	}
}

void init_audio() {
	uint32_t space_size = sizeof(uint32_t) * nr_reg;
	audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
	add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
	add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif
	audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
	sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
	temp_buf = (uint8_t *)new_space(TEMP_BUF_SIZE);
	add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, temp_buf, TEMP_BUF_SIZE, audio_buffer_handle);
}
