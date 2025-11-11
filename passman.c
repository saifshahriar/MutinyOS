#include <ncurses.h>
#include <string.h>

#include "passman.h"

#define MAX_ENTRIES 200
#define NAME_LEN    64
#define PASS_LEN    128

typedef struct {
	char name[NAME_LEN];
	char password[PASS_LEN];
} Entry;

static Entry entries[MAX_ENTRIES];
static int   entry_count = 0;
static char  master[PASS_LEN];

// color pair ids
#define CP_TITLE     1
#define CP_MENU      2
#define CP_BORDER    3
#define CP_HIGHLIGHT 4
#define CP_TEXT      5

static void draw_box(WINDOW *w, int color_pair) {
	wattron(w, COLOR_PAIR(color_pair));
	box(w, 0, 0);
	wattroff(w, COLOR_PAIR(color_pair));
}

static void print_center(WINDOW *w, int y, int width, const char *s,
                         int color_pair) {
	wattron(w, COLOR_PAIR(color_pair) | A_BOLD);
	mvwprintw(w, y, (width - (int)strlen(s)) / 2, "%s", s);
	wattroff(w, COLOR_PAIR(color_pair) | A_BOLD);
}

static void get_password_masked(WINDOW *w, int y, int x, char *buffer,
                                int max_len) {
	int idx = 0;
	int ch;
	wmove(w, y, x);
	wrefresh(w);
	keypad(w, TRUE);
	noecho();
	while (1) {
		ch = wgetch(w);
		if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
			break;
		} else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
			if (idx > 0) {
				idx--;
				buffer[idx] = '\0';
				mvwaddch(w, y, x + idx, ' ');
				wmove(w, y, x + idx);
			}
		} else if (ch >= 32 && ch <= 126 && idx < max_len - 1) {
			buffer[idx++] = (char)ch;
			mvwaddch(w, y, x + idx - 1, '*');
			wmove(w, y, x + idx);
		}
		wrefresh(w);
	}
	buffer[idx] = '\0';
}

static void set_master_password() {
	int     win_h = 7, win_w = 60;
	WINDOW *w = newwin(win_h, win_w, (LINES - win_h) / 2, (COLS - win_w) / 2);
	draw_box(w, CP_BORDER);
	print_center(w, 1, win_w, "SET MASTER PASSWORD (this run only)", CP_TITLE);
	mvwprintw(w, 3, 2, "Master: ");
	wrefresh(w);

	get_password_masked(w, 3, 10, master, PASS_LEN);
	mvwprintw(w, 4, 2, "Retype : ");
	for (int i = 10; i < win_w - 2; ++i)
		mvwaddch(w, 4, i, ' ');
	wmove(w, 4, 10);
	char confirm[PASS_LEN];
	get_password_masked(w, 4, 10, confirm, PASS_LEN);

	if (strcmp(master, confirm) != 0) {
		mvwprintw(w, 5, 2,
		          "Passwords do not match! Press any key to re-enter...");
		wrefresh(w);
		wgetch(w);
		delwin(w);
		set_master_password();
		return;
	}

	mvwprintw(w, 5, 2, "Master set for this run. Press any key...");
	wrefresh(w);
	wgetch(w);
	delwin(w);
}

static int check_master_prompt() {
	int     win_h = 7, win_w = 56;
	WINDOW *w = newwin(win_h, win_w, (LINES - win_h) / 2, (COLS - win_w) / 2);
	draw_box(w, CP_BORDER);
	print_center(w, 1, win_w, "ENTER MASTER PASSWORD", CP_TITLE);
	mvwprintw(w, 3, 2, "Password: ");
	wrefresh(w);

	char attempt[PASS_LEN];
	get_password_masked(w, 3, 12, attempt, PASS_LEN);

	int ok = (strcmp(attempt, master) == 0);
	if (!ok)
		mvwprintw(w, 5, 2, "Wrong master password! Press any key...");
	else
		mvwprintw(w, 5, 2, "Correct. Press any key...");
	wrefresh(w);
	wgetch(w);
	delwin(w);
	return ok;
}

static void add_entry() {
	int     win_h = 11, win_w = 64;
	WINDOW *w = newwin(win_h, win_w, (LINES - win_h) / 2, (COLS - win_w) / 2);
	draw_box(w, CP_BORDER);
	print_center(w, 1, win_w, "ADD PASSWORD", CP_TITLE);

	mvwprintw(w, 3, 2, "Name : ");
	echo();
	curs_set(1);
	char name[NAME_LEN];
	mvwgetnstr(w, 3, 10, name, NAME_LEN - 1);
	noecho();

	mvwprintw(w, 5, 2, "Pass : ");

	char pass[PASS_LEN];
	get_password_masked(w, 5, 10, pass, PASS_LEN);

	if (entry_count < MAX_ENTRIES) {
		strncpy(entries[entry_count].name, name, NAME_LEN - 1);
		entries[entry_count].name[NAME_LEN - 1] = '\0';
		strncpy(entries[entry_count].password, pass, PASS_LEN - 1);
		entries[entry_count].password[PASS_LEN - 1] = '\0';
		entry_count++;
		mvwprintw(w, 7, 2, "Saved. Press any key...");
	} else {
		mvwprintw(w, 7, 2, "Storage full! Press any key...");
	}

	wrefresh(w);
	wgetch(w);
	delwin(w);
	curs_set(0);
}

static void view_entries() {
	if (entry_count == 0) {
		int     wh = 5, ww = 40;
		WINDOW *w = newwin(wh, ww, (LINES - wh) / 2, (COLS - ww) / 2);
		draw_box(w, CP_BORDER);
		print_center(w, 1, ww, "NO PASSWORDS", CP_TITLE);
		mvwprintw(w, 2, 2, "Press any key to continue...");
		wrefresh(w);
		wgetch(w);
		delwin(w);
		return;
	}

	if (!check_master_prompt())
		return;

	int     win_h = LINES - 4;
	int     win_w = COLS - 8;
	WINDOW *w     = newwin(win_h, win_w, 2, 4);
	draw_box(w, CP_BORDER);
	print_center(w, 1, win_w, "SAVED PASSWORDS (MASTER UNLOCKED)", CP_TITLE);

	int max_display = win_h - 5;
	int start       = 0;
	keypad(w, TRUE);

	while (1) {
		for (int r = 2; r < win_h - 1; ++r)
			for (int c = 1; c < win_w - 1; ++c)
				mvwaddch(w, r, c, ' ');

		for (int i = 0; i < max_display && (start + i) < entry_count; ++i) {
			int idx = start + i;

			mvwprintw(w, 2 + i, 2, "%2d. %-30s : %s", idx + 1,
			          entries[idx].name, entries[idx].password);
		}

		mvwprintw(w, win_h - 2, 2, "Up/Down to scroll, q to exit");
		wrefresh(w);

		int ch = wgetch(w);
		if (ch == 'q' || ch == 'Q' || ch == 27)
			break;
		else if (ch == KEY_DOWN) {
			if (start + max_display < entry_count)
				start++;
		} else if (ch == KEY_UP) {
			if (start > 0)
				start--;
		} else if (ch == KEY_NPAGE) {
			if (start + max_display < entry_count)
				start += max_display;
			if (start + max_display > entry_count)
				start = entry_count - max_display;
			if (start < 0)
				start = 0;
		} else if (ch == KEY_PPAGE) {
			start -= max_display;
			if (start < 0)
				start = 0;
		}
	}

	delwin(w);
}

static void delete_entry() {
	if (entry_count == 0) {
		int     wh = 5, ww = 40;
		WINDOW *w = newwin(wh, ww, (LINES - wh) / 2, (COLS - ww) / 2);
		draw_box(w, CP_BORDER);
		print_center(w, 1, ww, "NO PASSWORDS", CP_TITLE);
		mvwprintw(w, 2, 2, "Press any key to continue...");
		wrefresh(w);
		wgetch(w);
		delwin(w);
		return;
	}

	if (!check_master_prompt())
		return;

	int     win_h = 9, win_w = 56;
	WINDOW *w = newwin(win_h, win_w, (LINES - win_h) / 2, (COLS - win_w) / 2);
	draw_box(w, CP_BORDER);
	print_center(w, 1, win_w, "DELETE PASSWORD", CP_TITLE);

	int show = entry_count < 4 ? entry_count : 4;
	for (int i = 0; i < show; ++i)
		mvwprintw(w, 2 + i, 2, "%2d. %-30s", i + 1, entries[i].name);
	mvwprintw(w, 6, 2, "Enter index to delete (1-%d): ", entry_count);
	echo();
	curs_set(1);
	int idx = 0;
	wscanw(w, "%d", &idx);
	noecho();
	curs_set(0);

	if (idx >= 1 && idx <= entry_count) {
		for (int i = idx - 1; i < entry_count - 1; ++i)
			entries[i] = entries[i + 1];
		entry_count--;
		mvwprintw(w, 7, 2, "Deleted. Press any key...");
	} else {
		mvwprintw(w, 7, 2, "Invalid index. Press any key...");
	}
	wrefresh(w);
	wgetch(w);
	delwin(w);
}

int passman() {
	initscr();
	start_color();
	use_default_colors();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	init_pair(CP_TITLE, COLOR_CYAN, -1);
	init_pair(CP_MENU, COLOR_YELLOW, -1);
	init_pair(CP_BORDER, COLOR_MAGENTA, -1);
	init_pair(CP_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
	init_pair(CP_TEXT, COLOR_WHITE, -1);

	set_master_password();

	while (1) {
		clear();
		attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
		mvprintw(1, (COLS - 26) / 2, "FANCY PASSWORD MANAGER");
		attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

		attron(COLOR_PAIR(CP_MENU));
		mvprintw(4, (COLS - 20) / 2, "1. Add Password");
		mvprintw(5, (COLS - 20) / 2, "2. View Passwords");
		mvprintw(6, (COLS - 20) / 2, "3. Delete Password");
		mvprintw(7, (COLS - 20) / 2, "4. Exit");
		attroff(COLOR_PAIR(CP_MENU));

		mvprintw(9, (COLS - 30) / 2, "Enter choice: ");
		refresh();

		int ch = getch();
		if (ch == '1' || ch == KEY_F(1))
			add_entry();
		else if (ch == '2' || ch == KEY_F(2))
			view_entries();
		else if (ch == '3' || ch == KEY_F(3))
			delete_entry();
		else if (ch == '4' || ch == 'q' || ch == 'Q')
			break;
	}

	endwin();
	return 0;
}
