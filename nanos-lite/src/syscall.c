#include <common.h>
#include "syscall.h"

#define STRACE

void sys_yield() {
	yield();
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
		case SYS_yield:
			sys_yield();
			c->GPRx = 0;
			break;
		default:
			panic("Unhandled syscall ID = %d", a[0]);
	}
#ifdef STRACE
	Log("%s(%d, %d, %d, %d) = %d", syscall_name[a[0]], a[0], a[1], a[2], a[3], c->GPRx);
#endif
}
