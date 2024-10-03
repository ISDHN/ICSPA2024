/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#ifndef __ISA_RISCV_H__
#define __ISA_RISCV_H__

#include <common.h>

typedef struct {
	word_t gpr[MUXDEF(CONFIG_RVE, 16, 32)]; // general purpose registers
	vaddr_t pc;
	bool intr;
	word_t mt_csr[256 + 1];
	word_t spv_csr[128 + 1];
	word_t *csr[16];
} MUXDEF(CONFIG_RV64, riscv64_CPU_state, riscv32_CPU_state);

typedef union {
	struct {
		word_t ppn : 22;
		word_t asid : 9;
		word_t mode : 1;
	};
	word_t val;
} satp_t;

typedef union {
	struct {
		word_t reserve_0 : 1;
		word_t sie : 1;
		word_t reserve_1 : 1;
		word_t mie : 1;
		word_t reserve_2 : 2;
		word_t spie : 1;
		word_t ube : 1;
		word_t mpie : 1;
		word_t spp : 1;
		word_t vs : 2;
		word_t mpp : 2;
		word_t fs : 2;
		word_t xs : 2;
		word_t mprv : 1;
		word_t sum : 1;
		word_t mxr : 1;
		word_t tvm : 1;
		word_t tw : 1;
		word_t tsr : 1;
		word_t reserve_3 : 8;
		word_t sd : 1;
	};
	word_t val;
} mstatus_t;

// decode
typedef struct {
	union {
		uint32_t val;
	} inst;
} MUXDEF(CONFIG_RV64, riscv64_ISADecodeInfo, riscv32_ISADecodeInfo);

enum mt_csr_cst { // mechine trap csr
	mstatus = 0,
	misa,
	medeleg,
	mideleg,
	mie,
	mtvec,
	mcounteren,
	mstatush,
	mscratch = 0x40,
	mepc,
	mcause,
	mtval,
	mip,
	mtinst,
	mtval2
};

enum spv_csr_cst {
	satp = 0x80
};

enum csr_kind_cst {
	srw = 1,
	mrw = 3,
	mro = 15
};

#define CSR(imm) cpu.csr[imm >> 8][imm & 0xff]
#define MRW(index) cpu.mt_csr[index]

#define isa_mmu_check(vaddr, len, type) (cpu.spv_csr[satp] >> 31 ? MMU_TRANSLATE : MMU_DIRECT)

#endif
