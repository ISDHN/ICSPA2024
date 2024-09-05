#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "expr.h"
#include <common.h>
#include <stdint.h>
#include <stdlib.h>

#define NR_WP 64

typedef struct watchpoint {
	char *expr;
	word_t val;
	bool occupied;
	/* TODO: Add more members if necessary */

} WP;
void init_wp_pool();
void add_wp(char *e);
void list_wps();
void rm_wp(int index);
#endif