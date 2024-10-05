#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
	[AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
	[AM_KEY_NONE] = "NONE",
	AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	for (size_t i = 0; i < len; i++) {
		putch(((char *)buf)[i]);
	}
	yield();
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	yield();
	if (ev.keycode == AM_KEY_NONE) {
		return 0;
	}
	return snprintf(buf, len, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	return snprintf(buf, len, "WIDTH: %d\nHEIGHT: %d\n", io_read(AM_GPU_CONFIG).width, io_read(AM_GPU_CONFIG).height);
}

static int screen_w;

size_t fb_write(const void *buf, size_t offset, size_t len) {
	int pix_idx = offset / sizeof(uint32_t);
	int x = pix_idx % screen_w;
	yield();
	int y = pix_idx / screen_w;
	int l = len / sizeof(uint32_t);

	io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, l, 1, true);
	return len;
}

size_t sbctrl_read(void *buf, size_t offset, size_t len) {
	if (len < sizeof(int)) {
		return 0;
	}
	AM_AUDIO_CONFIG_T cfg = io_read(AM_AUDIO_CONFIG);
	AM_AUDIO_STATUS_T st = io_read(AM_AUDIO_STATUS);
	*((int *)buf) = cfg.bufsize - st.count;
	return 4;
}

size_t sbctrl_write(const void *buf, size_t offset, size_t len) {
	if (len < 3 * sizeof(int)) {
		return 0;
	}
	io_write(AM_AUDIO_CTRL, ((int *)buf)[0], ((int *)buf)[1], ((int *)buf)[2]);
	return 3 * sizeof(int);
}

size_t sb_write(const void *buf, size_t offset, size_t len) {
	char *p = (char *)buf;
	io_write(AM_AUDIO_PLAY, {p, p + len});
	return len;
}

void init_device() {
	Log("Initializing devices...");
	ioe_init();
	screen_w = io_read(AM_GPU_CONFIG).width;
}
