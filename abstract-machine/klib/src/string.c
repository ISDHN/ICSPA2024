#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	const char *p = s;
	for (; *p != 0; p++)
		;
	return p - s;
}

char *strcpy(char *dst, const char *src) {
	size_t i = 0;
	for (; src[i] != 0; i++) {
		dst[i] = src[i];
	}
	dst[i] = 0;
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i = 0;
	for (; i < n && src[i] != 0; i++) {
		dst[i] = src[i];
	}
	for (; i < n; i++) {
		dst[i] = 0;
	}
	return dst;
}

char *strcat(char *dst, const char *src) {
	strcpy(dst + strlen(dst), src);
	return dst;
}

char *strncat(char *dst, const char *src, size_t n) {
	char *pos = dst + strlen(dst);
	size_t i = 0;
	for (; i < n && src[i] != 0; i++) {
		pos[i] = src[i];
	}
	pos[i] = 0;
	return dst;
}

int strcmp(const char *s1, const char *s2) {
	size_t i = 0;
	for (; s1[i] != 0 && s2[i] != 0; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] - s2[i];
		}
	}
	return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
	size_t i = 0;
	for (; i < n && s1[i] != 0 && s2[i] != 0; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] - s2[i];
		}
	}
	if (i == n) {
		return 0;
	}
	return s1[i] - s2[i];
}

void *memset(void *s, int c, size_t n) {
	for (int i = 0; i < n; i++) {
		((char *)s)[i] = c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	if (dst < src) {
		for (int i = 0; i < n; i++) {
			((char *)dst)[i] = ((char *)src)[i];
		}
	} else {
		for (int i = n - 1; i >= 0; i--) {
			((char *)dst)[i] = ((char *)src)[i];
		}
	}
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	for (int i = 0; i < n; i++) {
		((char *)out)[i] = ((char *)in)[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	for (int i = 0; i < n; i++) {
		if (((char *)s1)[i] != ((char *)s2)[i]) {
			return ((char *)s1)[i] - ((char *)s2)[i];
		}
	}
	return 0;
}

#endif
