#include <stdio.h>
#include <string.h>

#include "cmd.h"

// clang-format off
Prog cmds[] = {
	{ "help",       "",      "Show help text" },
	{ "cmd|cmds",   "",      "List all available commands" },
	{ "prog|progs", "",      "List all available programs" },
	{ "addusr",     "",      "Add another user to the system" },
	{ "bug",        "",      "Standard text editor for MutinyOS" },
	{ "calculator", "calc",  "Scientific calculator" },
	{ "calender",   "cal",   "Simple calender shows weeks and days of the current month" },
	{ "chat",       "",      "Open chat client" },
	{ "chusr",      "",      "Login to another user account" },
	{ "hostname",   "",      "Print effective host name" },
	{ "passman",    "",      "Password manager for MutinyOS" },
	{ "sudoku",     "",      "Games/sudoku" },
	{ "sysinfo",    "",      "Shows system information" },
	{ "tictactoe",  "",      "Games/tictactoe" },
	{ "users",      "",      "List all the users in a system" },
	{ "whoami",     "",      "Print effective username" },
	{ "sh",         "",      "MutinyOS's standard POSIX shell to control the system" },
	{ NULL,         NULL,    NULL }
};

Prog progs[] = {
	{ "bug",        "",      "Standard text editor for MutinyOS" },
	{ "calculator", "calc",  "Scientific calculator" },
	{ "calender",   "cal",   "Simple calender shows weeks and days of the current month" },
	{ "passman",    "",      "Password manager for MutinyOS" },
	{ "sudoku",     "",      "Games/sudoku" },
	{ "sysinfo",     "",      "Shows system information" },
	{ "tictactoe",  "",      "Games/tictactoe" },
	{ "sh",         "",      "MutinyOS's standard POSIX shell to control the system" },
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
