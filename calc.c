#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/compatibility.h"
#include "lib/tinyexpr.h"

#include "calc.h"
#include "cmd.h"

#define MAX_ARGS 20
#define MAX_CMD  100

void calc();
void eqn();
void help();
void mode();

typedef struct {
	char *name;
	char *desc;
} Mode;

Mode modes[] = {
	{ "math", "Basic math I/O mode" },
	{  "eqn",     "Equation solver" },
	{   NULL,	              NULL }
};

void mode() {
	/* color(GREEN, "Available modes:\n"); */
	printf("%-12s %-1s\n", "CMD", "DESCRIPTION");
	printf("---------------------------------\n");

	for (int i = 0; modes[i].name; ++i) {
		int space = abs(13 - (int)strlen(modes[i].name));
		color(GREEN, modes[i].name);
		for (int j = 0; j < space; ++j)
			printf(" ");
		color(CYAN, modes[i].desc);
		puts("");
	}
}

// main
void calc() {
	char line[MAX_CMD];

	while (1) {
		printf("[calculator | IN]> ");
		if (!fgets(line, sizeof(line), stdin))
			break;

		line[strcspn(line, "\n")] = '\0';

		if (cmd(line, "exit"))
			break;
		else if (cmd(line, "clear") || cmd(line, "clr") || cmd(line, "cls"))
			clrscr();
		else if (cmd(line, "mode"))
			mode();
		else if (!strlen(line))
			continue;
		else {
			double res = te_interp(line, 0);
			printf("[calculator | OUT]> %g\n", res);
		}
	}
}
