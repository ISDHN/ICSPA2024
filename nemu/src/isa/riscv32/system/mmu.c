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
#include <memory/vaddr.h>
#include <memory/paddr.h>

#pragma GCC push_options
#pragma GCC optimize(0)
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
	vaddr_ena va = (vaddr_ena)vaddr;
	satp_t satp_csr = (satp_t)cpu.spv_csr[satp];
	pte_t pte;
	int i, a;
	for (i = LEVEL - 1, a = satp_csr.ppn;; i--, a = pte.ppn) {
		if (i < 0) {
			panic("Page fault! Table walk too deeply! Type: %d", type);
		}
		word_t vpn = (va.vpn >> (i * 10)) & PN_MASK;
		pte = (pte_t)paddr_read((a << PAGE_SHIFT) | vpn << PTE_SHIFT, PTE_SIZE);
		if (!pte.valid) {
			panic("Invalid pte %#x when access %#x with method %c", (a << PAGE_SHIFT) | vpn << PTE_SHIFT, vaddr, "xrw"[type]);
		}
		if (pte.r | pte.ex) {
			break;
		}
	}
	paddr_t res;
	if (i == 1) {
		res = ((pte.ppn & ~(PN_MASK)) << PAGE_SHIFT) | (va.vpn & PN_MASK) << PAGE_SHIFT | va.pgoff;
	} else {
		res = pte.ppn << PAGE_SHIFT | va.pgoff;
	}
	return res;
}
#pragma GCC pop_options
