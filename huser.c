#include <crypt.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lib/compatibility.h"
#include "lib/util.h"

#include "global.h"
#include "register.h"

void chusr() {
	char username[BUFFSIZE];
	char password[BUFFSIZE];
	char line[BUFFSIZE * BUFFSIZE];
	int  attempts = 0;

rewind_prompt:
	printf("Switch to username: ");
	if (!fgets(username, sizeof(username), stdin)) {
		perror("Failed to read username");
		return;
	}
	rmnl(username);

	char *pw = getpass("Password: ");
	strncpy(password, pw, sizeof(password));
	password[sizeof(password) - 1] = '\0';

	FILE *sfp = fopen(SHADOWS_F, "r");
	if (!sfp) {
		perror("Failed to open shadows file");
		return;
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

		if (!strcmp(u, username)) {
			char *enc = crypt(password, SALT);
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
		printf("Login failed: invalid username or password.\n");
		if (++attempts < 3)
			goto rewind_prompt;
		printf("Too many failed attempts.\n");
		return;
	}

	if (chdir(globals.homedir) != 0)
		perror("Failed to change to home directory");

	printf("Switched to user: %s\n", globals.fullname);
}

void addusr() {
	puts("Adding a new user...");
	puts("-----------------");

	char fullname[BUFFSIZE];
	char username[BUFFSIZE];
	char password[BUFFSIZE];

	printf("Enter full name: ");
	if (!fgets(fullname, sizeof(fullname), stdin)) {
		perror("Failed to read full name");
		return;
	}
	rmnl(fullname);

	printf("Enter username: ");
	if (!fgets(username, sizeof(username), stdin)) {
		perror("Failed to read username");
		return;
	}
	rmnl(username);

	char *pw = getpass("Enter password: ");
	if (!pw) {
		fprintf(stderr, "Failed to read password\n");
		return;
	}
	strncpy(password, pw, sizeof(password));
	password[sizeof(password) - 1] = '\0';

	char *enc_pass = crypt(password, SALT);
	if (!enc_pass) {
		perror("Password encryption failed");
		return;
	}

	FILE *sfp = fopen(SHADOWS_F, "a");
	if (!sfp) {
		perror("Failed to write shadows file");
		return;
	}
	fprintf(sfp, "%s:%s:%s\n", username, fullname, enc_pass);
	fclose(sfp);

	char homedir[BUFFSIZE * 2];
	snprintf(homedir, sizeof(homedir), "%s/home/%s", BASE_DIR_PATH, username);
	if (mkdir(homedir, 0755) && errno != EEXIST)
		perror("Failed to create home directory");

	printf("User %s added successfully!\n", username);
}
