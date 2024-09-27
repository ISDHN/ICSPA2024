#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// #define AM

#ifdef AM
int main(const char *args);
#else
int main(int argc, char *argv[], char *envp[]);
#endif
extern char **environ;
extern void __libc_init_array(void);

void call_main(uintptr_t *args) {
	printf("\a\a\a\a\a\aargc in call main: ");
	// int argc = *(int *)args;
	//  char **argv = (char **)(args + 1);
	//  environ = argv + argc + 1;
	__libc_init_array();
#ifdef AM
	exit(main(am_arg));
#else
	exit(main(0, NULL, NULL));
#endif
	assert(0);
}
