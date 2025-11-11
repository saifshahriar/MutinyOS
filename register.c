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

static void create_home_dir() {
	snprintf(globals.homedir, sizeof(globals.homedir), "%s/home/%s",
	         BASE_DIR_PATH, globals.username);

	if (mkdir(globals.homedir, 0755) && errno != EEXIST)
		perror("Failed to create home directory");
}

static char *encrypt_pass(const char *raw_pass) {
	char *enc = crypt(raw_pass, SALT);
	if (!enc) {
		perror("Password encryption failed");
		return NULL;
	}
	return enc;
}

static void handle_pass() {
	char raw_pass[BUFFSIZE];

	char *pw = getpass("Enter password for your account: ");
	if (!pw) {
		fprintf(stderr, "Failed to read password\n");
		return;
	}

	strncpy(raw_pass, pw, sizeof(raw_pass));
	raw_pass[sizeof(raw_pass) - 1] = '\0';

	char *encrypted = encrypt_pass(raw_pass);
	if (!encrypted)
		return;

	strncpy(globals.password, encrypted, sizeof(globals.password));
	globals.password[sizeof(globals.password) - 1] = '\0';
}

static void save_hostname() {
	FILE *hfp = fopen(HOST_F, "w");
	if (!hfp) {
		perror("Failed to write hostname file");
		return;
	}
	fprintf(hfp, "%s\n", globals.hostname);
	fclose(hfp);
}

static void save_shadows() {
	FILE *sfp = fopen(SHADOWS_F, "a");
	if (!sfp) {
		perror("Failed to write shadows file");
		return;
	}

	if (isfempty(SETUP_F))
		fprintf(sfp, "root::root\n");

	fprintf(sfp, "%s:%s:%s\n", globals.username, globals.fullname,
	        globals.password);
	fclose(sfp);
}

void reg_prompt() {
	puts("Setting up for the first time...");
	puts("--------------------------------");

	printf("Enter your full name: ");
	if (!fgets(globals.fullname, sizeof(globals.fullname), stdin)) {
		perror("Failed to read full name");
		return;
	}
	rmnl(globals.fullname);

	printf("Enter username for your account: ");
	if (!fgets(globals.username, sizeof(globals.username), stdin)) {
		perror("Failed to read username");
		return;
	}
	rmnl(globals.username);

	handle_pass();

	printf("Enter hostname for your PC: ");
	if (!fgets(globals.hostname, sizeof(globals.hostname), stdin)) {
		perror("Failed to read hostname");
		return;
	}
	rmnl(globals.hostname);

	save_hostname();
	save_shadows();
	create_home_dir();

	color(GREEN, "User registration completed successfully!\n");
	puts("Press Enter to continue...");
	getchar();
}

int reg() {
	if (!isfempty(SETUP_F))
		return 0;

	reg_prompt();

	FILE *f = fopen(SETUP_F, "w");
	if (!f) {
		fprintf(stderr, "setup: error opening %s: %s\n", SETUP_F,
		        strerror(errno));
		return 0;
	}

	fprintf(f, "1");
	fclose(f);
	return 1;
}
