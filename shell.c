#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lib/compatibility.h"
#include "lib/util.h"

#include "cal.h"
#include "calc.h"
#include "chat.h"
#include "cmd.h"
#include "credit.h"
#include "date.h"
#include "global.h"
#include "help.h"
#include "hostname.h"
#include "huser.h"
#include "login.h"
#include "register.h"
#include "users.h"
#include "whoami.h"

#define MAX_ARGS 20
#define MAX_CMD  100

void prompt() {
	char cwd[BUFFSIZE * 5];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("cwd");
		printf("%s@%s%% ", globals.username, globals.hostname);
		return;
	}

	const char *display = cwd;
	if (!strncmp(cwd, BASE_DIR_PATH, strlen(BASE_DIR_PATH))) {
		display = cwd + strlen(BASE_DIR_PATH);
		if (*display == '/')
			display++;
	} else {
		if (chdir(globals.homedir) != 0) {
			perror("Failed to change to home directory");
			return;
		}
		color(RED, "Permission denied: moving back to ~\n");
		strncpy(cwd, globals.homedir, sizeof(cwd));
	}

	if (!strncmp(cwd, globals.homedir, strlen(globals.homedir))) {
		display = cwd + strlen(globals.homedir);
		printf("%s@%s:~%s%% ", globals.username, globals.hostname, display);
	} else
		printf("%s@%s:/%s%% ", globals.username, globals.hostname, display);
}

int main() {
	if (!reg())
		login_prompt();
	fetch_hostname();

	if (chdir(globals.homedir)) {
		perror("Failed to change to home directory");
		exit(1);
	}

	clrscr();
	printf("Hey %s!\n", globals.fullname);
	print_help();

	char line[MAX_CMD];
	while (1) {
		prompt();
		if (!fgets(line, sizeof(line), stdin))
			break;

		rmnl(line);

		/* skip empty lines */
		char *t = line;
		while (*t && isspace((unsigned char)*t))
			t++;
		if (!*t)
			continue;

		if (cmd(line, "exit"))
			break;
		else if (cmd(line, "clear") || cmd(line, "clr") || cmd(line, "cls"))
			clrscr();
		else if (cmd(line, "cmd") || cmd(line, "cmds"))
			print_cmds();
		else if (cmd(line, "prog") || cmd(line, "progs"))
			print_progs();
		else if (cmd(line, "help"))
			print_help();
		else if (cmd(line, "calc") || cmd(line, "calculator"))
			calc();
		else if (cmd(line, "date"))
			date();
		else if (cmd(line, "hostname"))
			hostname();
		else if (cmd(line, "whoami"))
			whoami();
		else if (cmd(line, "credit"))
			credit();
		else if (cmd(line, "chat"))
			chat_client();
		else if (cmd(line, "users"))
			users();
		else if (cmd(line, "chusr"))
			chusr();
		else if (cmd(line, "addusr"))
			addusr();
		else if (cmd(line, "cal"))
			cal();
		else {
			// tokenize
			char *argv[MAX_ARGS];
			int   argc  = 0;
			char *token = strtok(line, " ");
			while (token && argc < MAX_ARGS - 1) {
				argv[argc++] = token;
				token        = strtok(NULL, " ");
			}
			argv[argc] = NULL;

			/* handle cd */
			if (argc > 0 && strcmp(argv[0], "cd") == 0) {
				char *dir = (argc > 1) ? argv[1] : getenv("HOME");
				if (!dir)
					dir = "/";
				if (chdir(dir) != 0)
					fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
				continue;
			}

			/* fork */
			if (argc > 0) {
				pid_t pid = fork();
				if (pid < 0) {
					perror("fork failed");
				} else if (pid == 0) {
					execvp(argv[0], argv);
					fprintf(stderr, "exec failed: %s: %s\n", argv[0],
					        strerror(errno));
					exit(127);
				} else {
					int status;
					waitpid(pid, &status, 0);
				}
			}
		}
	}

	return 0;
}
