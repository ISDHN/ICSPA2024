#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int display_w = 0, display_h = 0;

extern int open(const char *pathname, int flags, int mode);

uint32_t NDL_GetTicks() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
	if (evtdev == -1)
		return 0;
	return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
	if (*w >= display_w || *h >= display_h) {
		printf("NDL_OpenCanvas: window size too large\n");
		exit(-1);
	}
	if (*w == 0 && *h == 0) {
		*w = display_w;
		*h = display_h;
	}
	if (getenv("NWM_APP")) {
		int fbctl = 4;
		fbdev = 5;
		screen_w = *w;
		screen_h = *h;
		char buf[64];
		int len = sprintf(buf, "%d %d", screen_w, screen_h);
		// let NWM resize the window and create the frame buffer
		write(fbctl, buf, len);
		while (1) {
			// 3 = evtdev
			int nread = read(3, buf, sizeof(buf) - 1);
			if (nread <= 0)
				continue;
			buf[nread] = '\0';
			if (strcmp(buf, "mmap ok") == 0)
				break;
		}
		close(fbctl);
	} else {
		fbdev = open("/dev/fb", 0, 0);
	}
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	if (fbdev == -1) {
		printf("NDL_DrawRect: frame buffer not initialized\n");
		return;
	}
	for (int i = 0; i < h; i++) {
		lseek(fbdev, (x + (y + i) * display_w) * sizeof(uint32_t), SEEK_SET);
		write(fbdev, pixels + (y + i) * screen_w, w * sizeof(uint32_t));
	}
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
	return 0;
}

int NDL_QueryAudio() {
	return 0;
}

int NDL_Init(uint32_t flags) {
	if (getenv("NWM_APP")) {
		evtdev = 3;
	} else {
		evtdev = open("/dev/events", 0, 0);
	}
	int disp_fd = open("/proc/dispinfo", 0, 0);
	char buf[128];
	read(disp_fd, buf, sizeof(buf));
	printf("dispinfo: %s\n", buf);
	sscanf(buf, "WIDTH:%d HEIGHT:%d", &display_w, &display_h);
	return 0;
}

void NDL_Quit() {
}
