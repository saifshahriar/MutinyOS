#include <ncurses.h>

#include "tic-tac-toe.h"

static char board[3][3];
static int  current_player = 1;

static void init_colors() {
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_RED, COLOR_BLACK);
}

static void draw_border(int starty, int startx, int height, int width) {
	attron(COLOR_PAIR(2));
	for (int i = startx; i < startx + width; i++) {
		mvaddch(starty, i, ACS_HLINE);
		mvaddch(starty + height - 1, i, ACS_HLINE);
	}
	for (int i = starty; i < starty + height; i++) {
		mvaddch(i, startx, ACS_VLINE);
		mvaddch(i, startx + width - 1, ACS_VLINE);
	}
	mvaddch(starty, startx, ACS_ULCORNER);
	mvaddch(starty, startx + width - 1, ACS_URCORNER);
	mvaddch(starty + height - 1, startx, ACS_LLCORNER);
	mvaddch(starty + height - 1, startx + width - 1, ACS_LRCORNER);
	attroff(COLOR_PAIR(2));
}

static void init_board() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			board[i][j] = ' ';
}

static void draw_board() {
	int start_y = 8;
	int start_x = (COLS - 13) / 2;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			int y = start_y + i * 2;
			int x = start_x + j * 4;

			mvaddch(y, x, ACS_ULCORNER);
			mvaddch(y, x + 1, ACS_HLINE);
			mvaddch(y, x + 2, ACS_HLINE);
			mvaddch(y, x + 3, ACS_URCORNER);

			mvaddch(y + 1, x, ACS_VLINE);

			if (board[i][j] == 'X') {
				attron(COLOR_PAIR(6) | A_BOLD);
				mvaddch(y + 1, x + 2, 'X');
				attroff(COLOR_PAIR(6) | A_BOLD);
			} else if (board[i][j] == 'O') {
				attron(COLOR_PAIR(5) | A_BOLD);
				mvaddch(y + 1, x + 2, 'O');
				attroff(COLOR_PAIR(5) | A_BOLD);
			} else {
				mvaddch(y + 1, x + 2, ' ');
			}

			mvaddch(y + 1, x + 3, ACS_VLINE);

			mvaddch(y + 2, x, ACS_LLCORNER);
			mvaddch(y + 2, x + 1, ACS_HLINE);
			mvaddch(y + 2, x + 2, ACS_HLINE);
			mvaddch(y + 2, x + 3, ACS_LRCORNER);
		}
	}
}

static int check_winner() {
	for (int i = 0; i < 3; i++) {
		if (board[i][0] != ' ' && board[i][0] == board[i][1]
		    && board[i][1] == board[i][2]) {
			return board[i][0] == 'X' ? 1 : 2;
		}
	}

	for (int i = 0; i < 3; i++) {
		if (board[0][i] != ' ' && board[0][i] == board[1][i]
		    && board[1][i] == board[2][i]) {
			return board[0][i] == 'X' ? 1 : 2;
		}
	}

	if (board[0][0] != ' ' && board[0][0] == board[1][1]
	    && board[1][1] == board[2][2]) {
		return board[0][0] == 'X' ? 1 : 2;
	}
	if (board[0][2] != ' ' && board[0][2] == board[1][1]
	    && board[1][1] == board[2][0]) {
		return board[0][2] == 'X' ? 1 : 2;
	}

	int empty_cells = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			if (board[i][j] == ' ')
				empty_cells++;
	}

	if (empty_cells == 0)
		return 3;

	return 0;
}

static void show_result(int winner) {
	clear();
	draw_border(0, 0, LINES, COLS);

	attron(COLOR_PAIR(2) | A_BOLD);
	mvprintw(2, (COLS - 12) / 2, "GAME OVER");
	attroff(COLOR_PAIR(2) | A_BOLD);

	draw_board();

	attron(COLOR_PAIR(3));
	mvprintw(16, (COLS - 20) / 2, "Result: ");

	if (winner == 1) {
		attron(COLOR_PAIR(6) | A_BOLD);
		printw("Player X Wins!");
		attroff(COLOR_PAIR(6) | A_BOLD);
	} else if (winner == 2) {
		attron(COLOR_PAIR(5) | A_BOLD);
		printw("Player O Wins!");
		attroff(COLOR_PAIR(5) | A_BOLD);
	} else {
		attron(COLOR_PAIR(4) | A_BOLD);
		printw("It's a Draw!");
		attroff(COLOR_PAIR(4) | A_BOLD);
	}
	attroff(COLOR_PAIR(3));

	attron(COLOR_PAIR(4));
	mvprintw(LINES - 3, (COLS - 20) / 2, "Press any key to continue");
	attroff(COLOR_PAIR(4));

	getch();
}

int tictactoe() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	init_colors();
	init_board();

	int game_over = 0;
	int winner    = 0;

	while (!game_over) {
		clear();
		draw_border(0, 0, LINES, COLS);

		attron(COLOR_PAIR(2) | A_BOLD);
		mvprintw(2, (COLS - 12) / 2, "TIC TAC TOE");
		attroff(COLOR_PAIR(2) | A_BOLD);

		// Show current player
		attron(COLOR_PAIR(3));
		mvprintw(4, (COLS - 15) / 2, "Current Player: ");
		if (current_player == 1) {
			attron(COLOR_PAIR(6) | A_BOLD);
			printw("X");
		} else {
			attron(COLOR_PAIR(5) | A_BOLD);
			printw("O");
		}
		attroff(COLOR_PAIR(6) | A_BOLD | COLOR_PAIR(5) | A_BOLD);

		draw_board();

		// Show controls
		attron(COLOR_PAIR(4));
		mvprintw(16, (COLS - 40) / 2,
		         "Controls: Use number keys 1-9 to place your mark");
		mvprintw(17, (COLS - 40) / 2,
		         "1=Top-Left, 2=Top-Mid, 3=Top-Right, 4=Mid-Left, etc.");
		mvprintw(18, (COLS - 20) / 2, "Press Q to quit");
		attroff(COLOR_PAIR(4));

		refresh();

		int ch = getch();
		if (ch == 'q' || ch == 'Q')
			break;

		if (ch >= '1' && ch <= '9') {
			int pos = ch - '1';
			int row = pos / 3;
			int col = pos % 3;

			if (board[row][col] == ' ') {
				board[row][col] = (current_player == 1) ? 'X' : 'O';

				winner = check_winner();
				if (winner > 0) {
					game_over = 1;
					show_result(winner);
				} else {
					current_player = 3 - current_player;
				}
			}
		}
	}

	endwin();
	return 0;
}
