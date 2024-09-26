#include <nterm.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);
static void builtin_echo(char *args);
static void builtin_exit(char *args);

struct {
	char *name;
	void (*func)(char *args);
} builtin_cmds[] = {
	{"echo", builtin_echo},
	{"exit", builtin_exit},
};

static void
sh_printf(const char *format, ...) {
	static char buf[256] = {};
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buf, 256, format, ap);
	va_end(ap);
	term->write(buf, len);
}

static void sh_banner() {
	sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
	sh_printf("sh> ");
}

static void builtin_echo(char *args) {
	sh_printf("%s\n", args);
}

static void builtin_exit(char *args) {
	exit(0);
}

static void sh_handle_cmd(const char *cmd) {
	int cmd_len = strlen(cmd);
	char *program = strtok((char *)cmd, " ");
	program[cmd_len - 1] = '\0';
	for (int i = 0; i < sizeof(builtin_cmds) / sizeof(builtin_cmds[0]); i++) {
		if (strcmp(program, builtin_cmds[i].name) == 0) {
			int pos_arg = strlen(program) + 1;
			builtin_cmds[i].func(pos_arg >= cmd_len ? NULL : program + pos_arg);
			return;
		}
	}
	if (execvp(program, NULL)) {
		sh_printf("'%s' is not recognized as an internal or external command,operable program or batch file.\n", program);
	}
}

void builtin_sh_run() {
	sh_banner();
	sh_prompt();

	while (1) {
		SDL_Event ev;
		if (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
				const char *res = term->keypress(handle_key(&ev));
				if (res) {
					sh_handle_cmd(res);
					sh_prompt();
				}
			}
		}
		refresh_terminal();
	}
}
