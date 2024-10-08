#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

#define STACK_SIZE (8 * PGSIZE)
#define PAGE_SHIFT 12
#define PAGE_MASK (-1ul << PAGE_SHIFT)

typedef union {
	uint8_t stack[STACK_SIZE] PG_ALIGN;
	struct {
		Context *cp;
		AddrSpace as;
		// we do not free memory, so use `max_brk' to determine when to call _map()
		uintptr_t max_brk;
		uint32_t priority;
	};
} PCB;

typedef void (*sig_handle)(int);

typedef struct {
	void *pdir;
	sig_handle handle;
} AlarmerInfo;

typedef void (*thread_entry)(void *arg);

extern PCB *current;

#endif

// --------------proc setup and schedule------------------
Context *schedule(Context *prev);
uintptr_t loader(PCB *pcb, const char *filename);
int naive_uload(PCB *pcb, const char *filename);
int context_uload(const char *filename, char *const argv[], char *const envp[], uint32_t priority, bool new_one);
void switch_fg(int index);

// --------------  syscall and signal  -------------------
#define SIGALRM 14
void register_timer_handle(sig_handle callback);
void do_syscall(Context *c);