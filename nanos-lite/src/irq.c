#include <common.h>
#include <proc.h>

#define MAX_CB_NR 32

static sig_handle callbacks[MAX_CB_NR];
static int sig_count = 0;

void register_timer_handle(sig_handle callback) {
	callbacks[sig_count++] = callback;
}

static Context *do_event(Event e, Context *c) {
	switch (e.event) {
		case EVENT_IRQ_TIMER:
			for (int i = 0; i < sig_count; i++) {
				callbacks[i](SIGALRM);
			}
			// Log("Tik tak");
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
