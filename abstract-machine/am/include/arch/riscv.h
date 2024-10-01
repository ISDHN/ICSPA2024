#ifndef ARCH_H__
#define ARCH_H__

#ifdef __riscv_e
#define NR_REGS 16
#else
#define NR_REGS 32
#endif

typedef struct Context {
	uintptr_t gpr[NR_REGS];
	uintptr_t mcause;
	uintptr_t mstatus;
	uintptr_t mepc;
	void *pdir;
} Context;

typedef union {
	struct {
		uint32_t valid : 1;
		uint32_t r : 1;
		uint32_t w : 1;
		uint32_t ex : 1;
		uint32_t user : 1;
		uint32_t global : 1;
		uint32_t accessed : 1;
		uint32_t dirty : 1;
		uint32_t rsw : 2;
		uint32_t ppn : 22;
	};
	uint32_t val;
} pte_t;

typedef union {
	struct {
		uint32_t pgoff : 12;
		uint32_t vpn_0 : 10;
		uint32_t vpn_1 : 10;
	};
	uint32_t val;
} vaddr_ena;

typedef union {
	struct {
		uint32_t pgoff : 12;
		uint32_t ppn_0 : 10;
		uint64_t ppn_1 : 12;
	};
	uint64_t val;
} paddr_ena;

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[10] // a0
#define GPR3 gpr[11] // a1
#define GPR4 gpr[12] // a2
#define GPRx gpr[10] // a0

#endif
