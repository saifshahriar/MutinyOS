#include <stdio.h>

#include "cmd.h"

void print_help() {
	// clang-format off
	puts("     e    e                 d8   ,e,                    ,88~-_   ,d88~~\\");
	puts("    d8b  d8b     888  888 _d88__  \"  888-~88e Y88b  /  d888   \\  8888    ");
	puts("   d888bdY88b    888  888  888   888 888  888  Y888/  88888    | `Y88b   ");
	puts("  / Y88Y Y888b   888  888  888   888 888  888   Y8/   88888    |  `Y88b, ");
	puts(" /   YY   Y888b  888  888  888   888 888  888    Y     Y888   /     8888 ");
	puts("/          Y888b \"88_-888  \"88_/ 888 888  888   /       `88_-~   \\__88P' ");
	puts("                                              _/");
	// clang-format on

	printf("Welcome to MutinyOS %s!\n", VERSION);
	puts("--------------------------");
	puts("To list all the available commands, type `cmd` or `cmds`.");
	puts("To list all the available programs, type `prog` or `progs`.");
	printf("Total program(s): %d, Total command(s): %d.\n\n\n", cnt_prog(),
	       cnt_cmd());
}
