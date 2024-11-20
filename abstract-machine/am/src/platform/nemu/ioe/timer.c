#include <am.h>
#include <nemu.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	uptime->us = (uint64_t)inl(RTC_ADDR) | (uint64_t)inl(RTC_ADDR + 4) << 32;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
	rtc->second = 8;
	rtc->minute = 19;
	rtc->hour = 19;
	rtc->day = 4;
	rtc->month = 1;
	rtc->year = 1145;
}
