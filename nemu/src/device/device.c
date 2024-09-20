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

#include <common.h>
#include <utils.h>
#include <device/alarm.h>
#include <time.h>
#include <signal.h>
#ifndef CONFIG_TARGET_AM
#include <SDL2/SDL.h>
#endif

void init_map();
void init_serial();
void init_timer();
void init_vga();
void init_i8042();
void init_audio();
void init_disk();
void init_sdcard();
void init_alarm();

void send_key(uint8_t, bool);
void vga_update_screen();

bool need_update = false;
timer_t timer;

void alarm_handle(sigval_t val) {
	need_update = true;
}

void device_update() {
	if (!need_update) {
		return;
	}
	need_update = false;
	IFDEF(CONFIG_HAS_VGA, vga_update_screen());

#ifndef CONFIG_TARGET_AM
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				nemu_state.state = NEMU_QUIT;
				break;
#ifdef CONFIG_HAS_KEYBOARD
			// If a key was pressed
			case SDL_KEYDOWN:
			case SDL_KEYUP: {
				uint8_t k = event.key.keysym.scancode;
				bool is_keydown = (event.key.type == SDL_KEYDOWN);
				send_key(k, is_keydown);
				break;
			}
#endif
			default:
				break;
		}
	}
#endif
}

void sdl_clear_event_queue() {
#ifndef CONFIG_TARGET_AM
	SDL_Event event;
	while (SDL_PollEvent(&event))
		;
#endif
}

void init_hpet() {
	struct sigevent sev;
	memset(&sev, 0, sizeof(sev));
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = alarm_handle;
	sev.sigev_value.sival_ptr = &timer;
	timer_create(CLOCK_REALTIME, &sev, &timer);

	struct itimerspec its;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 1000000000 / TIMER_HZ;
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 1000000000 / TIMER_HZ;
	timer_settime(timer, 0, &its, NULL);
}

void init_device() {
	IFDEF(CONFIG_TARGET_AM, ioe_init());
	init_map();

	init_hpet();

	IFDEF(CONFIG_HAS_SERIAL, init_serial());
	IFDEF(CONFIG_HAS_TIMER, init_timer());
	IFDEF(CONFIG_HAS_VGA, init_vga());
	IFDEF(CONFIG_HAS_KEYBOARD, init_i8042());
	IFDEF(CONFIG_HAS_AUDIO, init_audio());
	IFDEF(CONFIG_HAS_DISK, init_disk());
	IFDEF(CONFIG_HAS_SDCARD, init_sdcard());

	IFNDEF(CONFIG_TARGET_AM, init_alarm());
}
