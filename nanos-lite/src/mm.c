#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void *new_page(size_t nr_page) {
	void *old = pf;
	pf += nr_page * PGSIZE;
	return old;
}

#ifdef HAS_VME
static void *pg_alloc(int n) {
	int nr_page = n / PGSIZE;
	void *ret = new_page(nr_page);
	memset(ret, 0, n);
	return ret;
}
#endif

void free_page(void *p) {
	panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
	uint32_t old_vpage = current->max_brk >> PAGE_SHIFT;
	uint32_t new_vpage = brk >> PAGE_SHIFT;
	if (old_vpage != new_vpage) {
		void *new_ppage = new_page(1);
		map(&current->as, (void *)(new_vpage), new_ppage, 0);
	}
	current->max_brk = brk;
	return 0;
}

void init_mm() {
	pf = (void *)ROUNDUP(heap.start, PGSIZE);
	Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
	vme_init(pg_alloc, free_page);
#endif
}
