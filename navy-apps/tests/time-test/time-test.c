#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);

int main() {
	struct timeval last = {0, 0};
	while (1) {
		struct timeval now;
		_gettimeofday(&now, NULL);
		if (now.tv_sec * 10 + now.tv_usec / 100000 - last.tv_sec * 10 - last.tv_usec / 100000 > 5) {
			printf("tick");
		}
	}

	return 0;
}
