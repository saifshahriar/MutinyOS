#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "mkdir.h"

void mkdir_cmd(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: mkdir <dirname>\n");
		return;
	}

	for (int i = 1; i < argc; ++i)
		if (mkdir(argv[i], 0755) != 0)
			fprintf(stderr, "mkdir: %s: %s\n", argv[i], strerror(errno));
}
