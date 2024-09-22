#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

int w;
int h;

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t data = inl(VGACTL_ADDR);
	*cfg = (AM_GPU_CONFIG_T){
		.present = true, .has_accel = false, .width = data >> 16, .height = data & 0x0000ffff, .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	uint32_t *src = ctl->pixels;
	int h = ctl->h;
	int w = ctl->w;
	if (src != NULL) {
		for (int y = 0; y < h; y++) {
			uint32_t screen_y = y + ctl->y;
			uint32_t screen_pixel_row = screen_y * w;
			uint32_t src_pixel_row = y * w;
			for (int x = 0; x < w; x++) {
				fb[screen_pixel_row + x + ctl->x] = src[src_pixel_row + x];
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
	w = cfg.width;
	h = cfg.height;
}