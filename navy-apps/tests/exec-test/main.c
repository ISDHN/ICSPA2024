#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int flag = 0;
	int n = (argc >= 2 ? atoi(argv[1]) : 1);

	char buf[16];
	sprintf(buf, "%d", n + 1);
	flag = 1;
	execl("/bin/nterm", "/bin/nterm", NULL);
	return 0;
}
