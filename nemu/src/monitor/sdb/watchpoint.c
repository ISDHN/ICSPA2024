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

static WP wp_pool[NR_WP] = {};

void init_wp_pool() {
}

void add_wp(char *e) {
	bool success = true;
	word_t val = expr(e, &success);
	if (!success) {
		printf("Invalid expression\n");
		return;
	}

	for (int i = 0; i < NR_WP; i++) {
		if (wp_pool[i].occupied) {
			continue;
		}
		wp_pool[i].expr = strdup(e);
		wp_pool[i].val = val;
		printf("Watchpoint No.%d: %s , value is %u\n", i, e, val);
		return;
	}
	printf("No more slot to contain new watchpoint.\n");
}

void list_wps() {
	for (int i = 0; i < NR_WP; i++) {
		if (!wp_pool[i].occupied) {
			continue;
		}
		printf("Watchpoint No.%d: %s , value is %u\n", i, wp_pool[i].expr, wp_pool[i].val);
	}
}

void rm_wp(int index) {
	if (index >= NR_WP) {
		printf("Invalid index: %d\n", index);
		return;
	}
	if (!wp_pool[index].occupied) {
		printf("There is no watchpoint No.%d \n", index);
		return;
	}
	wp_pool[index].occupied = false;
}

bool check_wps() {
	bool changed = false;
	for (int i = 0; i < NR_WP; i++) {
		if (!wp_pool[i].occupied) {
			continue;
		}
		bool success = true;
		word_t val = expr(wp_pool[i].expr, &success);
		if (val != wp_pool[i].val) {
			printf("Watchpoint No.%d: %s\n", i, wp_pool[i].expr);
			printf("Old value: %u, New value: %u\n", wp_pool[i].val, val);
			wp_pool[i].val = val;
			changed = true;
		}
	}
	return changed;
}
/* TODO: Implement the functionality of watchpoint */
