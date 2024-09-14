#include <utils.h>

void push_int(int *stack, int *len, int val) {
	stack[*len] = val;
	(*len) += 1;
}

int pop_int(int *stack, int *len) {
	if (*len == 0) {
		panic("Calling Stack is empty");
		return -1;
	}
	(*len) -= 1;
	return stack[*len];
}
