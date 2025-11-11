#include "global.h"
#include "lib/util.h"
#include "register.h"
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int login_prompt() {
	char password[BUFFSIZE];
	char line[BUFFSIZE * BUFFSIZE];
	int  attempts = 0;

rewind_prompt:
	printf("Username: ");
	if (!fgets(globals.username, sizeof(globals.username), stdin)) {
		perror("Failed to read username");
		return -1;
	}
	rmnl(globals.username);

	char *pass = getpass("Password: ");
	strncpy(password, pass, sizeof(password));
	password[sizeof(password) - 1] = '\0';

	FILE *sfp = fopen(SHADOWS_F, "r");
	if (!sfp) {
		perror("Failed to open shadows file");
		return 0;
	}

	int success = 0;
	while (fgets(line, sizeof(line), sfp)) {
		rmnl(line);
		if (!strlen(line))
			continue;

		char *u = strtok(line, ":");
		char *f = strtok(NULL, ":");
		char *e = strtok(NULL, ":");

		if (!u || !f || !e)
			continue;

		if (!strcmp(u, globals.username)) {
			char *enc = crypt(password, SALT);   // same as registration
			if (enc && !strcmp(enc, e)) {
				strncpy(globals.username, u, sizeof(globals.username));
				strncpy(globals.fullname, f, sizeof(globals.fullname));
				strncpy(globals.password, e, sizeof(globals.password));
				snprintf(globals.homedir, sizeof(globals.homedir), "%s/home/%s",
				         BASE_DIR_PATH, globals.username);

				success = 1;
				break;
			}
		}
	}

	fclose(sfp);

	if (!success) {
		printf("Login failed: invalid username or password. Try again.\n");
		if (++attempts >= 3)
			exit(0);
		goto rewind_prompt;
	}

	// Move to home directory automatically
	if (chdir(globals.homedir) != 0) {
		perror("Failed to change to home directory");
		return 0;
	}

	printf("Welcome, %s!\n", globals.fullname);
	return 1;
}
