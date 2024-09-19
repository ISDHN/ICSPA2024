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
	if (length == 0) {
		return 0;
	}
	int cnt = 0;
#define PUTC(ch)                 \
	if (to_string) {             \
		if (cnt == length - 1) { \
			*out = 0;            \
			return cnt;          \
		}                        \
		SPUTCHAR(ch);            \
	} else {                     \
		PUTCHAR(ch);             \
	}                            \
	cnt++;

	for (const char *p = fmt; *p != 0; p++) {
		if (*p != '%') {
			PUTC(*p);
			continue;
		}
		p++;
		switch (*p) {
			case 'x':
			case 'u':
			case 'd': {
				bool hex = *p == 'x';
				bool sign = *p == 'd';
				unsigned int x = va_arg(ap, unsigned int);
				if (sign) {
					int signval = x;
					if (signval < 0) {
						PUTC('-');
						x = -signval;
					}
				}
				int base = hex ? 16 : 10;
				char buf[12];
				int i = 0;
				do {
					int bit = x % base;
					buf[i] = bit < 10 ? '0' + bit : 'a' + bit - 10;
					i++;
					x /= base;
				} while (x);
				while (i) {
					i--;
					PUTC(buf[i]);
				}
				break;
			}
			case 's': {
				const char *s = va_arg(ap, const char *);
				for (; *s != 0; s++) {
					PUTC(*s);
				}
				break;
			}
			case 'c': {
				char c = va_arg(ap, int);
				PUTC(c);
				break;
			}
			default: {
				break;
			}
		}
	}
	if (to_string) {
		*out = 0;
	}
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
	int cnt = vsprintf(out, fmt, ap);
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
