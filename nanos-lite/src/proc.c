#include <proc.h>
#include <debug.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
static int pcb_count = 0;
PCB *current = NULL;

void switch_boot_pcb() {
	current = &pcb_boot;
}

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
	char *ustack = new_page(8);
	char *str_buffer = ustack;
#define PUSH(x, type)       \
	ustack -= sizeof(type); \
	*(type *)ustack = x;

	int string_buffer_size = 0; // get the size of all str including \0

	char *const *p = envp;
	int envc = 0;
	if (envp) {
		for (; *p; p++) {
			envc++;
			string_buffer_size += strlen(*p) + 1;
		}
	}
	p = argv;
	int argc = 0;
	if (argv) {
		for (; *p; p++) {
			argc++;
			string_buffer_size += strlen(*p) + 1;
		}
	}
	ustack -= string_buffer_size;
	PUSH(0, int);
	printf("string_buffer_size: %d\n", string_buffer_size);

#define PUSH_STR(s)          \
	int len = strlen(s) + 1; \
	str_buffer -= len;       \
	strcpy(str_buffer, s);

	for (int i = envc - 1; i >= 0; i--) {
		PUSH_STR(envp[i]);
		printf("envp[%d]: %s\n", i, str_buffer);
		PUSH(str_buffer, char *);
	}
	PUSH(0, int);

	for (int i = argc - 1; i >= 0; i--) {
		PUSH_STR(argv[i]);
		PUSH(str_buffer, char *);
		printf("argv[%d]: %s\n", i, str_buffer);
	}
	PUSH(argc, int);

	pcb[pcb_count].cp->GPRx = (uintptr_t)ustack;
	pcb_count++;
	switch_boot_pcb();
	return 0;
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
	context_uload("/bin/exec-test", (char *[]){"/bin/exec-test", NULL}, NULL);
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
		printf("next pcb:%d\n", next - pcb);
		current = next;
	} else {
		if (pcb[pcb_count - 1].cp != NULL) {
			printf("next pcb:%d\n", pcb_count);
			current = pcb;
		}
	}
	return current->cp;
}
