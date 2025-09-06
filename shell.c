#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "calc.c"
#include "lib/compatibility.h"

#define MAX_ARGS 20
#define MAX_CMD  100

int main() {
	char line[MAX_CMD];

	while (1) {
		printf("name@desktop:$> ");
		if (!fgets(line, sizeof(line), stdin))
			break;

		line[strcspn(line, "\n")] = '\0';

		if (!strcmp(line, "exit")) {
			printf("name@desktop:$ [EXIT]> Exiting...\n");
			break;
		}

		if (!strcmp(line, "clear") || !strcmp(line, "clr")
		    || !strcmp(line, "cls")) {
			clrscr();
			continue;
		}

		if (!strcmp(line, "calc")) {
			calc();
			continue;
		}

		if (!strlen(line))
			continue;

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
	return 0;
}
