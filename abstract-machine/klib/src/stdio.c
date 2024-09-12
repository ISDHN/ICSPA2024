#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>
#include <stdbool.h>

#define PUTCHAR(ch) \
	putch(ch);

#define SPUTCHAR(ch) \
	*out++ = ch;

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int __printf(bool to_string, char *out, uint32_t length, const char *fmt, va_list ap) {
	int cnt = 0;
	if (length == 0) {
		return 0;
	}
#define PUTC(ch)             \
	if (cnt == length - 1) { \
		*out = 0;            \
		return cnt;          \
	}                        \
	if (to_string) {         \
		SPUTCHAR(ch);        \
	} else {                 \
		PUTCHAR(ch);         \
	}                        \
	cnt++;

	for (const char *p = fmt; *p != 0; p++) {
		if (*p != '%') {
			PUTC(*p);
			continue;
		}
		p++;
		switch (*p) {
			case 'd': {
				int x = va_arg(ap, int);
				if (x < 0) {
					PUTC('-');
					x = -x;
				}
				do {
					PUTC('0' + x % 10);
					x /= 10;
				} while (x);
				break;
			}
			case 's': {
				const char *s = va_arg(ap, const char *);
				for (; *s != 0; s++) {
					PUTC(*s);
				}
				break;
			}
			default: {
				break;
			}
		}
	}
	*out = 0;
	return cnt;
}

int printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int cnt = __printf(false, NULL, -1, fmt, ap);
	va_end(ap);
	return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	return __printf(true, out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int cnt = __printf(true, out, -1, fmt, ap);
	va_end(ap);
	return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int cnt = vsnprintf(out, n, fmt, ap);
	va_end(ap);
	return cnt;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
	return __printf(true, out, n, fmt, ap);
}

#endif
