#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int flag = 0;
	int n = (argc >= 2 ? atoi(argv[1]) : 1);
	printf("%s: argv[1] = %d flag: %d\n", argv[0], n, flag);

	char buf[16];
	sprintf(buf, "%d", n + 1);
	flag = 1;
	execl(argv[0], argv[0], buf, NULL);
	return 0;
}
