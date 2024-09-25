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
	char *empty[] = {NULL};
	char *am_arg = "mario";
	environ = empty;
	__libc_init_array();
#ifdef AM
	exit(main(am_arg));
#else
	exit(main(0, empty, empty));
#endif
	assert(0);
}
