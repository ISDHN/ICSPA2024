#include <common.h>

void init_mm(void);
void init_device(void);
void init_ramdisk(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

int main() {
	extern const char logo[];
	printf("%s", logo);
	Log("'Hello World!' from Nanos-lite");
	Log("Build time: %s, %s", __TIME__, __DATE__);

	init_mm();

	init_device();

	init_ramdisk();

#ifdef HAS_CTE
	init_irq();
#endif

	init_fs();

	init_proc();

	Log("Finish initialization");

#ifdef HAS_CTE
	asm volatile("csrs mstatus, %0" ::"r"(1 << 3)); // enable interrupt
	yield();
#endif

	panic("Should not reach here");
}
