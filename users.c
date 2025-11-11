#include <stdio.h>
#include <string.h>

#include "lib/compatibility.h"

#include "global.h"

void users() {
	FILE *f = fopen(SHADOWS_F, "r");
	if (!f) {
		perror(SHADOWS_F);
		return;
	}

	char line[1024];
	while (fgets(line, sizeof(line), f)) {
		line[strcspn(line, "\r\n")] = 0;

		char *token = strtok(line, ":");
		if (token)
			printf("%s\n", token);
	}

	fclose(f);
}
