#include <ctype.h>
#include <strproc.h>

bool is_number_str(char *str) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}