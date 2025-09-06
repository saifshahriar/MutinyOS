#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lib/tinyexpr.h"

#include "calc.h"

#define MAX_ARGS 20
#define MAX_CMD  100

void calc() {
	char line[MAX_CMD];

	while (1) {
		printf("name@desktop:$ [calc | IN]> ");
		if (!fgets(line, sizeof(line), stdin))
			break;

		line[strcspn(line, "\n")] = '\0';

		if (!strcmp(line, "exit")) {
			printf("name@desktop:$ [EXIT]> Exiting...\n");

			break;
		}

		if (!strlen(line))
			continue;

		double res = te_interp(line, 0);
		printf("name@desktop:$ [calc | OUT]> %g\n", res);

		// tokenize
		// char *argv[MAX_ARGS];
		// int argc = 0;
		// char *token = strtok(line, " ");
		// while (token && argc < MAX_ARGS - 1) {
		//     argv[argc++] = token;
		//     token        = strtok(NULL, " ");
		// }
		// argv[argc] = NULL;
		//
		// pid_t pid = fork();
		// if (!pid) {
		//     execvp(argv[0], argv);
		//     perror("exec failed");
		//     exit(1);
		// } else
		//     wait(NULL);
	}
}
