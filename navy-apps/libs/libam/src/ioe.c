#include <am.h>
#include <klib-macros.h>
#include <NDL.h>
#include <SDL.h>

void __am_timer_init();
void __am_gpu_init();
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) {
	cfg->present = true;
	cfg->has_rtc = true;
}
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true; }
static void __am_uart_config(AM_UART_CONFIG_T *cfg) { cfg->present = false; }
static void __am_net_config(AM_NET_CONFIG_T *cfg) { cfg->present = false; }
static void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) { cfg->present = false; }
static void __am_audio_init() {}
static void __am_audio_ctrl(AM_AUDIO_CTRL_T *) {};
static void __am_audio_status(AM_AUDIO_STATUS_T *) {};
static void __am_audio_play(AM_AUDIO_PLAY_T *) {};

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
	[AM_TIMER_CONFIG] = __am_timer_config,
	[AM_TIMER_RTC] = __am_timer_rtc,
	[AM_TIMER_UPTIME] = __am_timer_uptime,
	[AM_INPUT_CONFIG] = __am_input_config,
	[AM_INPUT_KEYBRD] = __am_input_keybrd,
	[AM_GPU_CONFIG] = __am_gpu_config,
	[AM_GPU_FBDRAW] = __am_gpu_fbdraw,
	[AM_GPU_STATUS] = __am_gpu_status,
	[AM_UART_CONFIG] = __am_uart_config,
	[AM_AUDIO_CONFIG] = __am_audio_config,
	[AM_AUDIO_CTRL] = __am_audio_ctrl,
	[AM_AUDIO_STATUS] = __am_audio_status,
	[AM_AUDIO_PLAY] = __am_audio_play,
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
	for (int i = 0; i < LENGTH(lut); i++)
		if (!lut[i])
			lut[i] = fail;
	SDL_Init(0);
	__am_gpu_init();
	__am_timer_init();
	__am_audio_init();
	return true;
}

void ioe_read(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }

// ------------------------- Time -----------------------------

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uptime->us = tv.tv_sec * 1000000 + tv.tv_usec;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
	rtc->second = 8;
	rtc->minute = 19;
	rtc->hour = 19;
	rtc->day = 4;
	rtc->month = 1;
	rtc->year = 1145;
}

// ----------------------- Input ----------------------------
void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	SDL_Event evt;
	int res = SDL_PollEvent(&evt);
	if (!res) {
		kbd->keydown = false;
		kbd->keycode = 0;
		return;
	}
	kbd->keydown = evt.type == SDL_KEYDOWN;
	kbd->keycode = evt.key.keysym.sym;
}

//------------------------ Nvidia ---------------------------
int gpu_w;
int gpu_h;

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	int w = 0;
	int h = 0;
	NDL_OpenCanvas(&w, &h);
	*cfg = (AM_GPU_CONFIG_T){
		.present = true, .has_accel = false, .width = w, .height = h, .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	NDL_DrawRect(ctl->pixels, ctl->x, ctl->y, ctl->w, ctl->h);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
	status->ready = true;
}

void __am_gpu_init() {
	AM_GPU_CONFIG_T cfg;
	__am_gpu_config(&cfg);
	gpu_w = cfg.width;
	gpu_h = cfg.height;
}