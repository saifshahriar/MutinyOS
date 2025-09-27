#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lib/compatibility.h"

#include "calc.h"
#include "cmd.h"
#include "date.h"
#include "global.h"
#include "help.h"

#define MAX_ARGS 20
#define MAX_CMD  100

int main() {
	char line[MAX_CMD];

	print_help();
	while (1) {
		printf("name@desktop:$> ");
		if (!fgets(line, sizeof(line), stdin))
			break;

		line[strcspn(line, "\n")] = '\0';

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
		else if (strlen(line) > 0) {   // for unreconized commands create a base
			                           // system fork
			// tokenize
			char *argv[MAX_ARGS];
			int   argc  = 0;
			char *token = strtok(line, " ");
			while (token && argc < MAX_ARGS - 1) {
				argv[argc++] = token;
				token        = strtok(NULL, " ");
			}
			argv[argc] = NULL;

			pid_t pid = fork();
			if (!pid) {
				execvp(argv[0], argv);
				perror("exec failed");
				exit(1);
			} else
				wait(NULL);
		}
	}
	return 0;
}
