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

char *strstp(char *str) {
	char *end = str + strlen(str) - 1;
	while (*str != 0 && isspace(*str)) {
		str++;
	}
	if (*str == 0) {
		return str;
	}
	while (end > str && isspace(*end)) {
		end--;
	}
	*(end + 1) = 0;
	return str;
}