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
