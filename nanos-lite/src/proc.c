#include <proc.h>
#include <fs.h>
#include <debug.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
static int pcb_count = 0;
PCB *current = NULL;

void switch_boot_pcb() {
	current = &pcb_boot;
}

int context_kload(thread_entry func, void *arg, uint32_t priority) {
	if (pcb_count >= MAX_NR_PROC) {
		Log("No more PCB space");
		return 1;
	}
	pcb[pcb_count].cp = kcontext((Area){pcb[pcb_count].stack, pcb[pcb_count].stack + STACK_SIZE}, func, arg);
	pcb[pcb_count].priority = priority;
	pcb_count++;
	return 0;
}

int context_uload(const char *filename, char *const argv[], char *const envp[], uint32_t priority, bool new_one) {
	int existence_test = fs_open(filename, 0, 0);
	if (existence_test == -1) {
		Log("Failed to load program from %s", filename);
		return -2;
	}

	PCB *dst_pcb = NULL;
	if (new_one) {
		if (pcb_count >= MAX_NR_PROC) {
			Log("No more PCB space");
			return -12; // ENOMEM 12 Cannot allocate memory
		}
		dst_pcb = pcb + pcb_count;
		pcb_count++;
	} else {
		dst_pcb = current;
	}
	// create the page and stack first,
	// and load program at end
	// to protect the args in .data seg

	protect(&dst_pcb->as);
	Log("%s's pdir: %p", filename, dst_pcb->as.ptr);
	// stack grow downsides, so the initial stack pointer should be the end of the page
	int pg_nr_stk = 8;
	void *ustack = new_page(pg_nr_stk) + pg_nr_stk * PGSIZE;
	Log("New proc's ustack: %p -> %p", ustack, dst_pcb->as.area.end - pg_nr_stk * PGSIZE);
	for (int i = 0; i < pg_nr_stk; i++) {
		map(&dst_pcb->as, dst_pcb->as.area.end + PGSIZE * (i - pg_nr_stk), ustack + i * PGSIZE, 0);
	}

	char *str_buffer = ustack;

#define PUSH(x, type)       \
	ustack -= sizeof(type); \
	*(type *)ustack = x;

	int string_buffer_size = 0; // get the size of all str including \0

#define PUSH_STR(s)          \
	int len = strlen(s) + 1; \
	str_buffer -= len;       \
	strcpy(str_buffer, s);

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
	ustack = str_buffer - string_buffer_size;
	PUSH(0, int);

	for (int i = envc - 1; i >= 0; i--) {
		PUSH_STR(envp[i]);
		Log("envp[%d]: %s\n", i, str_buffer);
		PUSH(str_buffer, char *);
	}
	PUSH(0, int);

	for (int i = argc - 1; i >= 0; i--) {
		PUSH_STR(argv[i]);
		Log("argv[%d]: %s", i, str_buffer);
		PUSH(str_buffer, char *);
	}

	PUSH(argc, int);

	Log("Loading program: %s ...", filename);
	void *entry = (void *)loader(dst_pcb, filename);
	dst_pcb->cp = ucontext(&dst_pcb->as, (Area){dst_pcb->stack, dst_pcb->stack + STACK_SIZE}, entry);
	dst_pcb->cp->GPRx = (uintptr_t)ustack;
	dst_pcb->max_brk = 0;
	if (priority != -1) {
		dst_pcb->priority = priority;
	}
	switch_boot_pcb();
	return 0;
}

void hello_fun(void *arg) {
	int j = 1;
	while (1) {
		Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
		j++;
	}
}

void init_proc() {
	char *init_program = "/bin/nterm";
	context_kload(hello_fun, "arg10", 1);
	context_uload("/bin/hello", (char *[]){"/bin/hello", NULL}, NULL, 8, true);
	// context_kload(hello_fun, "arg1", 1);
	context_uload(init_program, (char *[]){init_program, NULL}, NULL, 128, true);
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
	return pcb + pcb_count - 1; // if boot pcb: return the first
}

Context *schedule(Context *prev) {
	static int count = 0;
	count++;
	current->cp = prev;
	if (current->priority == 0 || count % current->priority == 0) {
		PCB *next = get_next_proc();
		if (next != NULL) {
			current = next;
		}
	}
	return current->cp;
}
