#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct watchpoint {
	int NO;
	char *expr;
	word_t val;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */

} WP;
void init_wp_pool();
void add_wp(char *e);
#endif