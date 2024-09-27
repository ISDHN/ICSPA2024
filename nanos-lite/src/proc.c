#include <proc.h>
#include <debug.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
static int pcb_count = 0;
PCB *current = NULL;

int context_kload(thread_entry func, void *arg) {
	if (pcb_count >= MAX_NR_PROC) {
		Log("No more PCB space");
		return 1;
	}
	pcb[pcb_count].cp = kcontext((Area){pcb[pcb_count].stack, pcb[pcb_count].stack + STACK_SIZE}, func, arg);
	pcb_count++;
	return 0;
}

int context_uload(const char *filename, char *const argv[], char *const envp[]) {
	if (pcb_count >= MAX_NR_PROC) {
		Log("No more PCB space");
		return 2;
	}
	void *entry = (void *)loader(pcb + pcb_count, filename);
	if (!entry) {
		Log("Failed to load program from %s", filename);
		return 1;
	}
	pcb[pcb_count].cp = ucontext(NULL, (Area){pcb[pcb_count].stack, pcb[pcb_count].stack + STACK_SIZE}, entry);
	char *ustack = heap.end;

#define PUSH(x)          \
	ustack -= sizeof(x); \
	*ustack = x;

#define PUSH_STR(s)          \
	int len = strlen(s) + 1; \
	ustack -= len;           \
	strcpy(ustack, s);       \
	*p = ustack;

	char *const *p = envp;
	int envc = 0;
	if (envp) {
		for (; *p; p++)
			envc++;
	}

	PUSH(0);
	for (int i = envc - 1; i >= 0; i--) {
		PUSH((uintptr_t)envp[i]);
		printf("envp[%d]: %s\n", i, envp[i]);
	}

	p = argv;
	int argc = 0;
	if (argv) {
		for (; *p; p++)
			argc++;
	}

	PUSH(0);
	for (int i = argc - 1; i >= 0; i--) {
		PUSH((uintptr_t)argv[i]);
		printf("argv[%d]: %s\n", i, argv[i]);
	}
	PUSH(argc);

	pcb[pcb_count].cp->GPRx = (uintptr_t)ustack;
	printf("ustack: %p data: %d\n", ustack, *(int *)ustack);
	pcb_count++;
	return 0;
}

void switch_boot_pcb() {
	current = &pcb_boot;
}

void hello_fun(void *arg) {
	int j = 1;
	while (1) {
		// Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
		j++;
		yield();
	}
}

void init_proc() {
	context_kload(hello_fun, "Welcome");
	context_uload("/bin/pal", (char *[]){"--skip", NULL}, NULL);
	switch_boot_pcb();

	Log("Initializing processes...");

	// load program here
	// char *filename = "/bin/nterm";
	// Log("Loading program from %s", filename);
	// naive_uload(NULL, filename);
}

PCB *get_next_proc() {
	for (int i = 0; i < pcb_count; i++) {
		if (pcb + i == current) {
			return pcb + (i + 1) % pcb_count;
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
			current = pcb;
		}
	}
	return current->cp;
}
