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
	uintptr_t old_vpage = current->max_brk & PAGE_MASK;
	uintptr_t new_vpage = brk & PAGE_MASK;
	if (old_vpage != new_vpage && current->max_brk != 0) {
		int increment = (new_vpage - old_vpage) >> PAGE_SHIFT;
		void *new_ppage = new_page(increment);
		Log("brk: %p -> %p", old_vpage, new_vpage);
		for (int i = 0; i < increment; i++) {
			map(&current->as, (void *)old_vpage + (i + 1) * PGSIZE, new_ppage + i * PGSIZE, 0);
		}
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
