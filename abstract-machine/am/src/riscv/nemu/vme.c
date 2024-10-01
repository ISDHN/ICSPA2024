#include <am.h>
#include <nemu.h>
#include <klib.h>

#define PAGE_SHIFT 12
#define LEVEL 2
#define PAGE_SIZE (1ul << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)

#define PN_MASK 0x3ff

#define PTE_SHIFT 2
#define PTE_SIZE (1ul << PTE_SHIFT)

static AddrSpace kas = {};
static void *(*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;

static Area segments[] = { // Kernel memory mappings
	NEMU_PADDR_SPACE};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
	uintptr_t mode = 1ul << (__riscv_xlen - 1);
	asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
	uintptr_t satp;
	asm volatile("csrr %0, satp" : "=r"(satp));
	return satp << 12;
}

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *)) {
	pgalloc_usr = pgalloc_f;
	pgfree_usr = pgfree_f;

	kas.ptr = pgalloc_f(PGSIZE);

	int i;
	for (i = 0; i < LENGTH(segments); i++) {
		void *va = segments[i].start;
		for (; va < segments[i].end; va += PGSIZE) {
			map(&kas, va, va, 0);
		}
	}

	printf("Set satp.ppn to %p\n", kas.ptr);
	set_satp(kas.ptr);
	vme_enable = 1;

	return true;
}

void protect(AddrSpace *as) {
	PTE *updir = (PTE *)(pgalloc_usr(PGSIZE));
	as->ptr = updir;
	as->area = USER_SPACE;
	as->pgsize = PGSIZE;
	// map kernel space
	memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
	c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
	if (vme_enable && c->pdir != NULL) {
		set_satp(c->pdir);
	}
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
	uintptr_t root = (uintptr_t)as->ptr;
	vaddr_ena vaddr = {.val = (uintptr_t)va};
	if (as->area.start == USER_SPACE.start && as->area.end == USER_SPACE.end) {

	} else {
		pte_t *pte = (pte_t *)(root + (vaddr.vpn_1 << PTE_SHIFT));
		pte->ppn = ((uintptr_t)pa >> PAGE_SHIFT) & (~PN_MASK);
		pte->r = 1;
		pte->w = 1;
		pte->ex = 1;
		pte->valid = 1;
		// printf("Register entry:%p with %x \n", pte, pte->val);
	}
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
	Context *c = (Context *)kstack.end - 1;
	if (entry) {
		c->mepc = (uintptr_t)entry - 4;
	}
	*((Context **)kstack.start) = c;
	return c;
}
