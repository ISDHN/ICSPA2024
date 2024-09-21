#include <unistd.h>
#include <stdio.h>
#include <NDL.h>

int main() {
	NDL_Init(0);
	uint32_t last = NDL_GetTicks();
	while (1) {
		uint32_t now = NDL_GetTicks();
		if (now - last >= 500) {
			printf("tick\n");
			last = now;
		}
	}

	return 0;
}
