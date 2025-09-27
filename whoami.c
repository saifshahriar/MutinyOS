#include <stdio.h>
#include <string.h>

#include "global.h"
#include "whoami.h"

static const char *path = "/var/tmp/mutiny/username";

void fetch_username() {
	FILE *f = fopen(path, "r");

	if (!f) {
		perror("username: Error opening file");
		return;
	}

	if (!fgets(globals.username, BUFFSIZE, f))
		perror("Error reading username");
	fclose(f);
	globals.username[strcspn(globals.username, "\n")] = '\0';
}

void whoami() {
	printf("%s\n", globals.username);
}
