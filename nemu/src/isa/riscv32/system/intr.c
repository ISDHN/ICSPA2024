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

#include <isa.h>

static inline mstatus_t *get_mstatus() {
	return (mstatus_t *)(&cpu.mt_csr[mstatus]);
}

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
	cpu.csr[mrw][mepc] = epc;
	cpu.csr[mrw][mcause] = NO;
	mstatus_t *status = get_mstatus();
	status->mpie = status->mie;
	status->mie = 0;
	// if (cpu.mt_csr[mcause] == 0x80000007) {
	// 	Log("Start timer intr");
	// 	isa_reg_display();
	// }
#ifdef CONFIG_ETRACE
	Log("Interrupt: NO = %#x, epc = %#x, mcause = %#x", NO, epc, cpu.csr[mrw][mcause]);
	Log("Go to interrupt handler: %#x", cpu.csr[mrw][mtvec]);
#endif
	return cpu.csr[mrw][mtvec];
}

word_t isa_query_intr() {
	mstatus_t *status = get_mstatus();
	if (status->mie && cpu.intr) {
		cpu.intr = false;
		return IRQ_TIMER;
	}
	return INTR_EMPTY;
}
