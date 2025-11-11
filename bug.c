#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES     1000
#define MAX_COLS      256
#define STATUS_HEIGHT 3

typedef struct {
	char **lines;
	int    line_count;
	int    cursor_x, cursor_y;
	int    scroll_y, scroll_x;
	char   status_msg[80];
	char   filename[256];
	int    dirty;
} Editor;

Editor editor;

static void init_editor() {
	editor.lines         = malloc(MAX_LINES * sizeof(char *));
	editor.lines[0]      = calloc(MAX_COLS, 1);
	editor.line_count    = 1;
	editor.cursor_x      = 0;
	editor.cursor_y      = 0;
	editor.scroll_y      = 0;
	editor.scroll_x      = 0;
	editor.status_msg[0] = '\0';
	editor.filename[0]   = '\0';
	editor.dirty         = 0;
}

static void free_editor() {
	for (int i = 0; i < editor.line_count; i++)
		free(editor.lines[i]);
	free(editor.lines);
}

static void set_status(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(editor.status_msg, sizeof(editor.status_msg), fmt, args);
	va_end(args);
}

static void insert_char(int y, int x, char c) {
	if (y >= editor.line_count)
		return;

	char *line = editor.lines[y];
	int   len  = strlen(line);

	if (len >= MAX_COLS - 1)
		return;

	memmove(&line[x + 1], &line[x], len - x + 1);
	line[x]      = c;
	editor.dirty = 1;
}

static void delete_char(int y, int x) {
	if (y >= editor.line_count)
		return;

	char *line = editor.lines[y];
	int   len  = strlen(line);

	if (x >= len)
		return;

	memmove(&line[x], &line[x + 1], len - x);
	editor.dirty = 1;
}

static void insert_line(int y) {
	if (editor.line_count >= MAX_LINES - 1)
		return;

	memmove(&editor.lines[y + 1], &editor.lines[y],
	        (editor.line_count - y) * sizeof(char *));

	editor.lines[y] = calloc(MAX_COLS, 1);
	editor.line_count++;
	editor.dirty = 1;
}

static void delete_line(int y) {
	if (editor.line_count <= 1)
		return;

	free(editor.lines[y]);

	memmove(&editor.lines[y], &editor.lines[y + 1],
	        (editor.line_count - y - 1) * sizeof(char *));
	editor.line_count--;
	editor.dirty = 1;
}

static int load_file(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file)
		return 0;

	for (int i = 0; i < editor.line_count; i++)
		free(editor.lines[i]);

	char line[MAX_COLS];
	editor.line_count = 0;

	while (fgets(line, sizeof(line), file) && editor.line_count < MAX_LINES) {
		line[strcspn(line, "\n")] = '\0';

		editor.lines[editor.line_count] = malloc(MAX_COLS);
		strncpy(editor.lines[editor.line_count], line, MAX_COLS - 1);
		editor.lines[editor.line_count][MAX_COLS - 1] = '\0';
		editor.line_count++;
	}

	if (editor.line_count == 0) {
		editor.lines[0]   = calloc(MAX_COLS, 1);
		editor.line_count = 1;
	}

	fclose(file);
	strncpy(editor.filename, filename, sizeof(editor.filename) - 1);
	editor.dirty = 0;
	return 1;
}

static int save_file(const char *filename) {
	FILE *file = fopen(filename, "w");
	if (!file)
		return 0;

	for (int i = 0; i < editor.line_count; i++)
		fprintf(file, "%s\n", editor.lines[i]);

	fclose(file);
	strncpy(editor.filename, filename, sizeof(editor.filename) - 1);
	editor.dirty = 0;
	return 1;
}

static void show_help() {
	def_prog_mode();
	endwin();

	printf("\n=== Text Editor - Keyboard Shortcuts ===\n\n");
	printf("File Operations:\n");
	printf("  Ctrl+O  - Open file\n");
	printf("  Ctrl+S  - Save file\n");
	printf("  Ctrl+N  - New file\n");
	printf("  Ctrl+Q  - Quit\n\n");

	printf("Editing:\n");
	printf("  Ctrl+Z  - Undo\n");
	printf("  Ctrl+X  - Cut line\n");
	printf("  Ctrl+C  - Copy line\n");
	printf("  Ctrl+V  - Paste line\n");
	printf("  Ctrl+D  - Delete line\n");
	printf("  Tab     - Indent\n");
	printf("  Shift+Tab - Unindent\n\n");

	printf("Navigation:\n");
	printf("  Arrow keys - Move cursor\n");
	printf("  Home/End   - Line start/end\n");
	printf("  Page Up/Dn - Scroll\n");
	printf("  Ctrl+A     - Beginning of file\n\n");

	printf("Press Enter to continue...");
	getchar();

	reset_prog_mode();
	refresh();
}

static void draw_editor(WINDOW *win) {
	int height, width;
	getmaxyx(win, height, width);

	werase(win);

	for (int y = 0; y < height - STATUS_HEIGHT; y++) {
		int line_num = editor.scroll_y + y;
		if (line_num < editor.line_count) {
			char *line = editor.lines[line_num];
			int   len  = strlen(line);

			int display_start = editor.scroll_x;
			if (display_start < len)
				mvwaddnstr(win, y, 0, line + display_start, width);
		}
	}

	wattron(win, A_REVERSE);
	mvwprintw(win, height - 3, 0, "%-*s", width, editor.status_msg);
	wattroff(win, A_REVERSE);

	char info[256];
	snprintf(info, sizeof(info), "File: %s | Lines: %d | %s",
	         editor.filename[0] ? editor.filename : "[Untitled]",
	         editor.line_count, editor.dirty ? "MODIFIED" : "Saved");

	mvwprintw(win, height - 2, 0, "%-*s", width, info);

	mvwprintw(win, height - 1, 0,
	          "F1:Help  Ctrl+S:Save  Ctrl+O:Open  Ctrl+N:New  Ctrl+Q:Quit");

	int cursor_screen_y = editor.cursor_y - editor.scroll_y;
	int cursor_screen_x = editor.cursor_x - editor.scroll_x;

	if (cursor_screen_y >= 0 && cursor_screen_y < height - STATUS_HEIGHT
	    && cursor_screen_x >= 0 && cursor_screen_x < width) {
		wmove(win, cursor_screen_y, cursor_screen_x);
	}

	wrefresh(win);
}

static void handle_keypress(int ch) {
	switch (ch) {
		/* Same switch logic as before */
		/* omitted for brevity — unchanged */
	}
}

static void update_scroll(int height) {
	if (editor.cursor_y < editor.scroll_y)
		editor.scroll_y = editor.cursor_y;
	else if (editor.cursor_y >= editor.scroll_y + height - STATUS_HEIGHT)
		editor.scroll_y = editor.cursor_y - (height - STATUS_HEIGHT) + 1;

	if (editor.cursor_x < editor.scroll_x)
		editor.scroll_x = editor.cursor_x;
	else if (editor.cursor_x >= editor.scroll_x + COLS)
		editor.scroll_x = editor.cursor_x - COLS + 1;
}

int bug(/* int argc, char *argv[]*/) {
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(1);

	init_editor();

	/* if (argc > 1) */
	/* 	if (!load_file(argv[1])) */
	/* 		set_status("Cannot open file: %s", argv[1]); */
	/* 	else */
	/* 		set_status("Loaded: %s", argv[1]); */
	/* else */
	/* 	set_status("Welcome to Text Editor! F1 for help, Ctrl+S to save"); */

	int running = 1;
	while (running) {
		update_scroll(LINES);
		draw_editor(stdscr);

		int ch = getch();

		if (ch == 17) {
			if (editor.dirty) {
				set_status(
				    "Unsaved changes! Press Ctrl+Q again to quit " "with" "out " "savi" "ng");
				ch = getch();
				if (ch == 17)
					running = 0;
			} else {
				running = 0;
			}
		} else {
			handle_keypress(ch);
		}
	}

	free_editor();
	endwin();
	return 0;
}
