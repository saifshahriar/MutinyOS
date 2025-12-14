#include <stdio.h>
#include <string.h>

#include "cmd.h"

// clang-format off
Prog cmds[] = {
	{ "help",       "",      "Show help text" },
	{ "cmd|cmds",   "",      "List all available commands" },
	{ "prog|progs", "",      "List all available programs" },
	{ "addusr",     "",      "Add another user to the system" },
	{ "calculator", "calc",  "Scientific calculator" },
	{ "calender",   "cal",   "Simple calender shows weeks and days of the current month" },
	{ "cd",         "",      "Change directory" },
	{ "chusr",      "",      "Login to another user account" },
	{ "clear",      "cls",   "Clears the terminal buffer" },
	{ "credit",     "",      "The mind behind MutinyOS" },
	{ "date",       "",      "Shows current date and time" },
	{ "hostname",   "",      "Print effective host name" },
	{ "ls",         "",      "List directory" },
	{ "mkdir",      "",      "Create new directory" },
	{ "passman",    "",      "Password manager for MutinyOS" },
	{ "rm",         "",      "Remove a file or directory" },
	{ "sh",         "",      "MutinyOS's standard POSIX shell to control the system" },
	{ "snake",      "",      "Games/snake" },
	{ "sudoku",     "",      "Games/sudoku" },
	{ "tasker",     "",      "Quick task manager from the terminal" },
	{ "ted",        "",      "Standard text editor for MutinyOS" },
	{ "tictactoe",  "",      "Games/tictactoe" },
	{ "touch",      "",      "Create new file" },
	{ "users",      "",      "List all the users in a system" },
	{ "whoami",     "",      "Print effective username" },
	{ NULL,         NULL,    NULL }
};

Prog progs[] = {
	{ "calculator", "calc",  "Scientific calculator" },
	{ "calender",   "cal",   "Simple calender shows weeks and days of the current month" },
	{ "snake",      "",      "Games/snake" },
	{ "sudoku",     "",      "Games/sudoku" },
	{ "tasker",     "",      "Quick task manager from the terminal" },
	{ "ted",        "",      "Standard text editor for MutinyOS" },
	{ "tictactoe",  "",      "Games/tictactoe" },
	{ NULL,         NULL,    NULL }
};
// clang-format on

int cmd(char *inp, char *cmd) {
	if (!strcmp(inp, cmd))
		return 1;
	return 0;
}

int cnt_cmd() {
	int i = 0;
	while (cmds[i].cmd)
		++i;
	return i;
}

int cnt_prog() {
	int i = 0;
	while (progs[i].cmd)
		++i;
	return i;
}

void print_cmds() {
	printf("%-12s %-6s %s\n", "CMD", "ALIAS", "DESCRIPTION");
	printf("---------------------------------------------\n");

	for (int i = 0; cmds[i].cmd; ++i)
		printf("%-12s %-6s %s\n", cmds[i].cmd, cmds[i].alias, cmds[i].desc);
}

void print_progs() {
	printf("%-12s %-6s %s\n", "CMD", "ALIAS", "DESCRIPTION");
	printf("---------------------------------------------\n");

	for (int i = 0; progs[i].cmd; ++i)
		printf("%-12s %-6s %s\n", progs[i].cmd, progs[i].alias, progs[i].desc);
}
