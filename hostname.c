#include <stdio.h>
#include <string.h>

#include "global.h"
#include "hostname.h"

void fetch_hostname() {
	FILE *f = fopen(HOST_F, "r");

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
