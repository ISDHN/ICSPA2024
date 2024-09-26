#include <proc.h>
#include <debug.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(thread_entry func, void *arg) {
	static int pcb_count = 0;
	if (pcb_count >= MAX_NR_PROC) {
		Log("No more PCB space");
		return;
	}
	pcb[pcb_count].cp = kcontext((Area){pcb[pcb_count].stack, pcb[pcb_count].stack + STACK_SIZE}, func, arg);
	pcb_count++;
}

void switch_boot_pcb() {
	current = &pcb_boot;
}

void hello_fun(void *arg) {
	int j = 1;
	while (1) {
		Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
		j++;
		yield();
	}
}

void init_proc() {
	context_kload(hello_fun, "Fuck");
	context_kload(hello_fun, "you");
	switch_boot_pcb();

	Log("Initializing processes...");

	// load program here
	// char *filename = "/bin/nterm";
	// Log("Loading program from %s", filename);
	// naive_uload(NULL, filename);
}

PCB *get_next_proc() {
	for (int i = 0; i < MAX_NR_PROC; i++) {
		if (&pcb[i] == current) {
			if (i == MAX_NR_PROC - 1) {
				return &pcb[0];
			} else {
				return &pcb[i + 1];
			}
		}
	}
	return NULL; // boot pcb
}

Context *schedule(Context *prev) {
	current->cp = prev;
	PCB *next = get_next_proc();
	if (next != NULL) {
		current = next;
	} else {
		if (pcb[0].cp != NULL) {
			current = &pcb[0];
		}
	}
	return current->cp;
}
