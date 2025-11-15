/* util.c */
#include <stdio.h>
#include <string.h>

#include "util.h"

int isfempty(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f)
		return -1;

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fclose(f);

	return size == 0;
}

void rmnl(char *line) {
	line[strcspn(line, "\n")] = '\0';
}

int tokenize(char *line, char *argv[], int max_args) {
	int   argc  = 0;
	char *token = strtok(line, " ");
	while (token && argc < max_args - 1) {
		argv[argc++] = token;
		token        = strtok(NULL, " ");
	}
	argv[argc] = NULL;
	return argc;
}
