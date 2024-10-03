#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
// #define AM

#ifdef AM
int main(const char *args);
#else
int main(int argc, char *argv[], char *envp[]);
#endif
extern char **environ;
extern void __libc_init_array(void);

void call_main(uintptr_t args) {
	sbrk(0); // tell the system where the _end is
	int argc = *(int *)args;
	char **argv = (char **)(args + sizeof(int));
	environ = argv + argc + 1;
	__libc_init_array();
#ifdef AM
	exit(main(am_arg))
#else
	exit(main(argc, argv, NULL));
#endif
		assert(0);
}
