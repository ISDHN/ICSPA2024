#include <common.h>
#include "syscall.h"
#include <proc.h>
#include <fs.h>

#define STRACE
#define SYS_DISPATCH(name, ...)            \
	case SYS_##name:                       \
		c->GPRx = sys_##name(__VA_ARGS__); \
		break

struct timeval {
	long tv_sec;  /* seconds */
	long tv_usec; /* microseconds */
};

int sys_yield() {
	yield();
	return 0;
}

long sys_write(int fd, const void *buf, size_t count) {
	return fs_write(fd, buf, count);
}

long sys_read(int fd, void *buf, size_t count) {
	return fs_read(fd, buf, count);
}

int sys_open(const char *pathname, int flags, int mode) {
	int res = fs_open(pathname, flags, mode);
	if (res == -1) {
		panic("file not found: %s", pathname);
	}
	return res;
}

int sys_lseek(int fd, int offset, int whence) {
	return fs_lseek(fd, offset, whence);
}

int sys_brk(int new_brk) {
	return 0;
}

int sys_close(int fd) {
	return fs_close(fd);
}

int sys_gettimeofday(struct timeval *tv, void *tz) {
	if (tv != NULL) {
		AM_TIMER_UPTIME_T uptime = io_read(AM_TIMER_UPTIME);
		tv->tv_sec = uptime.us / 1000000;
		tv->tv_usec = uptime.us % 1000000;
	}
	return 0;
}

int sys_execve(const char *pathname, const char *const argv[], const char *const envp[]) {
	return naive_uload(NULL, pathname);
}

void do_syscall(Context *c) {
	uintptr_t a[4];
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
#ifdef STRACE
	Log("%s(%d, %d, %d): ", syscall_name[a[0]], a[1], a[2], a[3]);
#endif
	switch (a[0]) {
		case SYS_exit:
			// sys_execve("/bin/nterm", NULL, NULL);
			break;
			SYS_DISPATCH(write, a[1], (const char *)a[2], a[3]);
			SYS_DISPATCH(read, a[1], (char *)a[2], a[3]);
			SYS_DISPATCH(yield);
			SYS_DISPATCH(lseek, a[1], a[2], a[3]);
			SYS_DISPATCH(brk, a[1]);
			SYS_DISPATCH(open, (const char *)(a[1]), a[2], a[3]);
			SYS_DISPATCH(close, a[1]);
			SYS_DISPATCH(gettimeofday, (struct timeval *)a[1], (void *)a[2]);
			SYS_DISPATCH(execve, (const char *)a[1], (const char **)a[2], (const char **)a[3]);
		default:
			panic("Unhandled syscall ID = %d", a[0]);
	}
#ifdef STRACE
	Log("Result: %d", c->GPRx);
#endif
}
