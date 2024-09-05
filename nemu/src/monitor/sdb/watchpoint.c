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
#include "watchpoint.h"
#include "sdb.h"

#define NR_WP 64

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
	int i;
	for (i = 0; i < NR_WP; i++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
	}

	head = NULL;
	free_ = wp_pool;
}

void add_wp(char *e) {
	bool success = true;
	word_t val = expr(e, &success);
	if (!success) {
		printf("Invalid expression\n");
		return;
	}

	WP *wp = free_;
	if (wp == NULL) {
		printf("No enough watchpoints\n");
		return;
	}

	free_ = free_->next;
	wp->next = head;
	head = wp;

	strcpy(wp->expr, e);
	wp->val = val;
	printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
}
/* TODO: Implement the functionality of watchpoint */
