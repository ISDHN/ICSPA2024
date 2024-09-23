#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

int gpu_w;
int gpu_h;

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t data = inl(VGACTL_ADDR);
	*cfg = (AM_GPU_CONFIG_T){
		.present = true, .has_accel = false, .width = data >> 16, .height = data & 0x0000ffff, .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	uint32_t *src = ctl->pixels;
	int w = ctl->w, h = ctl->h;
	if (ctl->pixels != NULL) {
		for (int y = 0; y < h; y++) {
			int y_bias = y + ctl->y;
			for (int x = 0; x < w; x++) {
				fb[y_bias * gpu_w + x + ctl->x] = src[y * w + x];
			}
		}
	}
	if (ctl->sync) {
		outl(SYNC_ADDR, 1);
	}
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
	status->ready = true;
}

void __am_gpu_init() {
	int i;
	AM_GPU_CONFIG_T cfg;
	__am_gpu_config(&cfg);
	gpu_w = cfg.width;
	gpu_h = cfg.height;
}