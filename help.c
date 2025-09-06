#include <stdio.h>

#include "cmd.h"

void print_help() {
	printf("Welcome to MutinyOS %s!\n", VERSION);
	puts("------------------------");
	puts("To list all the available commands, type `cmd` or `cmds`.");
	puts("To list all the available programs, type `prog` or `progs`.");
	printf("Total program(s): %d, Total command(s): %d.\n\n\n", cnt_cmd(),
	       cnt_prog());
}
