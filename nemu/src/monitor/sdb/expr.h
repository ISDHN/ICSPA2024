#ifndef __EXPR_H__
#define __EXPR_H__
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
	TK_NOTYPE = 256,
	TK_EQ,

	/* TODO: Add more token types */

};

typedef struct _rule {
	const char *regex;
	int token_type;
} rule;

word_t expr(char *e, bool *success);

#endif