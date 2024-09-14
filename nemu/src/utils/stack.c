#include <utils.h>

void push_int(int *stack, int *len, int val) {
	stack[*len] = val;
	(*len) += 1;
}

int pop_int(int *stack, int *len) {
	(*len) -= 1;
	return stack[*len];
}
