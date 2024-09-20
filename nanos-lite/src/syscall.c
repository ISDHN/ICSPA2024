#include <common.h>
#include "syscall.h"

// #define STRACE
#define SYS_DISPATCH(name, ...)            \
	case SYS_##name:                       \
		c->GPRx = sys_##name(__VA_ARGS__); \
		break

int sys_yield() {
	yield();
}

long sys_write(int fd, const void *buf, size_t count) {
	if (fd != 1 && fd != 2) {
		return -1;
	}
	for (size_t i = 0; i < count; i++) {
		putch(((char *)buf)[i]);
	}
	return count;
}

void do_syscall(Context *c) {
	uintptr_t a[4];
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

	switch (a[0]) {
		case SYS_exit:
			halt(a[1]);
			break;
			SYS_DISPATCH(yield);
			SYS_DISPATCH(write, a[1], (const char *)a[2], a[3]);
		default:
			panic("Unhandled syscall ID = %d", a[0]);
	}
#ifdef STRACE
	Log("%s(%d, %d, %d) = %d", syscall_name[a[0]], a[1], a[2], a[3], c->GPRx);
#endif
}
