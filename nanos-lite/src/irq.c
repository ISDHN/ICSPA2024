#include <common.h>
#include <proc.h>

#define MAX_CB_NR 32

static AlarmerInfo callbacks[MAX_CB_NR];
static int sig_count = 0;

void __am_switch(Context *c);

void register_timer_handle(sig_handle callback) {
	Log("Registering timer handle %p in as %p", callback, current->as.ptr);
	callbacks[sig_count].handle = callback;
	callbacks[sig_count].pdir = current->as.ptr;
	sig_count++;
}

static Context *do_event(Event e, Context *c) {
	switch (e.event) {
		case EVENT_IRQ_TIMER:
			for (int i = 0; i < sig_count; i++) {
				Log("Calling timer handle %p in as %p", callbacks[i].handle, callbacks[i].pdir);
				__am_switch(callbacks[i].pdir);
				callbacks[i].handle(SIGALRM);
			}
		case EVENT_YIELD:
			c = schedule(c);
			break;
		case EVENT_SYSCALL:
			do_syscall(c);
			break;
		case EVENT_PAGEFAULT:
			Log("PAGEFAULT");
			break;
		default:
			panic("Unhandled event ID = %d", e.event);
	}

	return c;
}

void init_irq(void) {
	Log("Initializing interrupt/exception handler...");
	cte_init(do_event);
}
