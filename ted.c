#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ted.h"

#define MAX_LINES 1000
#define MAX_LEN   256

char *lines[MAX_LINES];
int   line_count = 0;

static void insert_line(const char *text) {
	if (line_count >= MAX_LINES) {
		printf("Buffer full!\n");
		return;
	}
	lines[line_count] = strdup(text);
	line_count++;
}

static void delete_line(int n) {
	if (n < 1 || n > line_count) {
		printf("Invalid line number\n");
		return;
	}
	free(lines[n - 1]);
	for (int i = n - 1; i < line_count - 1; i++)
		lines[i] = lines[i + 1];
	line_count--;
}

static void print_lines() {
	for (int i = 0; i < line_count; i++)
		printf("%d: %s\n", i + 1, lines[i]);
}

static void save_file(const char *filename) {
	FILE *f = fopen(filename, "w");
	if (!f) {
		perror("save");
		return;
	}
	for (int i = 0; i < line_count; i++)
		fprintf(f, "%s\n", lines[i]);
	fclose(f);
	printf("Saved %s\n", filename);
}

static void load_file(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		perror("open");
		return;
	}
	char buf[MAX_LEN];
	while (fgets(buf, sizeof(buf), f) && line_count < MAX_LINES) {
		buf[strcspn(buf, "\n")] = 0;
		insert_line(buf);
	}
	fclose(f);
}

int ted(int argc, char *argv[]) {
	char cmd[16];
	char fname[128] = { 0 };

	if (argc > 1) {
		strncpy(fname, argv[1], sizeof(fname) - 1);
		load_file(fname);
	}

	printf(
	    "Tiny Editor (TEd). Commands: i <text>, d <num>, p, s <file>, o " "<" "f" "i" "l" "e>, " "q" "\n");

	while (1) {
		printf("> ");
		if (!fgets(cmd, sizeof(cmd), stdin))
			break;

		cmd[strcspn(cmd, "\n")] = 0;

		if (cmd[0] == 'i' && cmd[1] == ' ') {
			insert_line(cmd + 2);
		} else if (cmd[0] == 'd' && cmd[1] == ' ') {
			int n = atoi(cmd + 2);
			delete_line(n);
		} else if (strcmp(cmd, "p") == 0) {
			print_lines();
		} else if (cmd[0] == 's' && cmd[1] == ' ') {
			strcpy(fname, cmd + 2);
			save_file(fname);
		} else if (cmd[0] == 'o' && cmd[1] == ' ') {
			strcpy(fname, cmd + 2);
			line_count = 0;   // clear buffer
			load_file(fname);
		} else if (strcmp(cmd, "q") == 0) {
			break;
		} else {
			printf("Unknown command\n");
		}
	}

	for (int i = 0; i < line_count; i++)
		free(lines[i]);

	return 0;
}
