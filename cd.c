#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "global.h"

#include "cd.h"

void cd_cmd(int argc, char **argv) {
	char *dir;

	if (argc < 2 || !argv[1])
		dir = getenv(globals.homedir);
	else
		dir = argv[1];

	if (!dir)
		dir = "/";

	if (chdir(dir) != 0)
		fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
}
