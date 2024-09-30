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

#ifndef __MEMORY_VADDR_H__
#define __MEMORY_VADDR_H__

#include <common.h>

word_t vaddr_ifetch(vaddr_t addr, int len);
word_t vaddr_read(vaddr_t addr, int len);
void vaddr_write(vaddr_t addr, int len, word_t data);

typedef union {
	struct {
		word_t mode : 1;
		word_t asid : 9;
		word_t ppn : 22;
	};
	word_t val;
} satp_t;

typedef union {
	struct {
		word_t ppn : 22;
		word_t rsw : 2;
		word_t dirty : 1;
		word_t accessed : 1;
		word_t global : 1;
		word_t user : 1;
		word_t ex : 1;
		word_t w : 1;
		word_t r : 1;
		word_t valid : 1;
	};
	word_t val;
} pte_t;

typedef union {
	struct {
		word_t vpn : 20;
		word_t pgoff : 12;
	};
	word_t val;
} vaddr_ena;

#define PAGE_SHIFT 12
#define LEVEL 2
#define PAGE_SIZE (1ul << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)

#define PN_MASK 0x3ff

#define PTE_SHIFT 2
#define PTE_SIZE (1ul << PTE_SHIFT)

#endif
