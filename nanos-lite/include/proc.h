#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

#define STACK_SIZE (8 * PGSIZE)
#define PAGE_SHIFT 12
#define PAGE_MASK ~(1ul << PAGE_SHIFT)

typedef union {
	uint8_t stack[STACK_SIZE] PG_ALIGN;
	struct {
		Context *cp;
		AddrSpace as;
		// we do not free memory, so use `max_brk' to determine when to call _map()
		uintptr_t max_brk;
	};
} PCB;

typedef void (*thread_entry)(void *arg);

extern PCB *current;

#endif

Context *schedule(Context *prev);
uintptr_t loader(PCB *pcb, const char *filename) __attribute__((optimize(0)));
int naive_uload(PCB *pcb, const char *filename);
int context_uload(const char *filename, char *const argv[], char *const envp[], bool new_one);
