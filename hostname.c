#include <stdio.h>
#include <string.h>

#include "global.h"
#include "hostname.h"

static const char *path = "/var/tmp/mutiny/hostname";

void fetch_hostname() {
	FILE *f = fopen(path, "r");

	if (!f) {
		perror("hostname: Error opening file");
		return;
	}

	if (!fgets(globals.hostname, BUFFSIZE, f))
		perror("Error reading hostname");
	fclose(f);
	globals.hostname[strcspn(globals.hostname, "\n")] = '\0';
}

void hostname() {
	printf("%s\n", globals.hostname);
}
