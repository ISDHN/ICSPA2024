#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <riscv/riscv.h>

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);
static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c) {
	__am_get_cur_as(c);
	if (user_handler) {
		Event ev = {0};
		if (c->mcause == -1) {
			ev.event = EVENT_YIELD;
		} else if (c->mcause >= 0 && c->mcause <= 19) {
			ev.event = EVENT_SYSCALL;
		} else if (c->mcause == 0x80000007) {
			ev.event = EVENT_IRQ_TIMER;
		} else {
			ev.event = EVENT_ERROR;
			printf("cause: %p, status: %p", c->mcause, c->mstatus);
			panic("fail");
		}
		c = user_handler(ev, c);
		assert(c != NULL);
	}
	if (c->pdir) {
		__am_switch(c);
	}
	return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *)) {
	// initialize exception entry
	asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
	// register event handler
	user_handler = handler;

	return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
	Context *c = (Context *)kstack.end - 1;
	c->mepc = (uintptr_t)entry - 4;
	c->mstatus = 0x1880;
	c->pdir = NULL;
	*((Context **)kstack.start) = c;
	c->GPRx = (uintptr_t)arg;
	return c;
}

void yield() {
#ifdef __riscv_e
	asm volatile("li a5, -1; ecall");
#else
	asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
	uint32_t mstatus;
	asm volatile("csrr %0,mstatus" : "=r"(mstatus) :);
	return mstatus & 8; // mie
}

void iset(bool enable) {
	if (enable) {
		asm volatile("csrs mstatus, %0" ::"r"(1 << 3)); // enable interrupt
	} else {
		asm volatile("csrc mstatus, %0" ::"r"(1 << 3)); // disable interrupt
	}
}
