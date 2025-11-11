#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/compatibility.h"
#include "lib/tinyexpr.h"
#include "lib/util.h"

#include "calc.h"
#include "cmd.h"

Mode modes[] = {
	{ "math", "Basic math I/O mode" },
	{  "eqn",     "Equation solver" },
	{   NULL,	              NULL }
};

void eqn() {
	int ch = getch();
	do {
		printf("Select the type of equations:\n");
		printf("[1] Simul");
	} while (ch == '1' || ch == '2');

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, 1);
	nodelay(stdscr, 1);

	char buf[6][32] = { { 0 } };   // a1, b1, c1, a2, b2, c2
	int  pos        = 0;
	int  idx[6]     = { 0 };

	int r, c;
	getmaxyx(stdscr, r, c);

	while (1) {
		/* get current terminal width */
		int rows, cols;
		getmaxyx(stdscr, rows, cols);

		if (r != rows || c != cols) {
			clear();
			getmaxyx(stdscr, r, c);
		}
		box(stdscr, 0, 0);   // draw border

		mvaddstr(0, 1, "Equation Solver");

		/* draw the equations */
		mvprintw(1, 1, "%s", buf[0]);
		mvprintw(1, 1 + (int)strlen(buf[0]), "x + ");
		mvprintw(1, 5 + (int)strlen(buf[0]), "%s", buf[1]);
		mvprintw(1, 5 + (int)strlen(buf[0]) + (int)strlen(buf[1]), "y = ");
		mvprintw(1, 9 + (int)strlen(buf[0]) + (int)strlen(buf[1]), "%s",
		         buf[2]);

		mvprintw(2, 1, "%s", buf[3]);
		mvprintw(2, 1 + (int)strlen(buf[3]), "x + ");
		mvprintw(2, 5 + (int)strlen(buf[3]), "%s", buf[4]);
		mvprintw(2, 5 + (int)strlen(buf[3]) + (int)strlen(buf[4]), "y = ");
		mvprintw(2, 9 + (int)strlen(buf[3]) + (int)strlen(buf[4]), "%s",
		         buf[5]);

		/* cursor placement */
		int cx, cy;
		switch (pos) {
		case 0:
			cx = 1 + idx[0];
			cy = 1;
			break;
		case 1:
			cx = 5 + (int)strlen(buf[0]) + idx[1];
			cy = 1;
			break;
		case 2:
			cx = 9 + (int)strlen(buf[0]) + (int)strlen(buf[1]) + idx[2];
			cy = 1;
			break;
		case 3:
			cx = 1 + idx[3];
			cy = 2;
			break;
		case 4:
			cx = 5 + (int)strlen(buf[3]) + idx[4];
			cy = 2;
			break;
		case 5:
			cx = 9 + (int)strlen(buf[3]) + (int)strlen(buf[4]) + idx[5];
			cy = 2;
			break;
		default:
			cx = 0;
			cy = 0;
			break;
		}
		move(cy, cx);

		refresh();

		ch = getch();
		if (ch == 27 || ch == 'q') {
			endwin();
			return;
		} else if (ch == '\t' || ch == 'w' || ch == KEY_RIGHT)
			pos = (pos + 1) % 6;
		else if (ch == 'b' || ch == KEY_LEFT)
			pos = (pos - 1) % 6;
		else if (ch == '\n') {
			endwin();
			solve_eqn2x2(atof(buf[0]), atof(buf[1]), atof(buf[2]), atof(buf[3]),
			             atof(buf[4]), atof(buf[5]));
			return;
		} else if (ch == KEY_BACKSPACE || ch == 127) {
			if (idx[pos] > 0)
				buf[pos][--idx[pos]] = '\0';
		} else if ((ch >= '0' && ch <= '9') || ch == '-' || ch == '+') {
			if (idx[pos] < (int)sizeof(buf[pos]) - 1) {
				buf[pos][idx[pos]++] = ch;
				buf[pos][idx[pos]]   = '\0';
			}
		}
	}
}

void mode() {
	/* color(GREEN, "Available modes:\n"); */
	printf("%-12s %-1s\n", "CMD", "DESCRIPTION");
	printf("---------------------------------\n");

	for (int i = 0; modes[i].name; ++i) {
		int space = abs(13 - (int)strlen(modes[i].name));
		color(GREEN, modes[i].name);
		for (int j = 0; j < space; ++j)
			printf(" ");
		color(CYAN, modes[i].desc);
		puts("");
	}
}

/* function to solve a 2x2 system: a1x + b1y = c1, a2x + b2y = c2 */
void solve_eqn2x2(double a1, double b1, double c1, double a2, double b2,
                  double c2) {
	double det = a1 * b2 - a2 * b1;
	if (det == 0) {
		printf("No unique solution (determinant = 0)\n");
		return;
	}
	double x = (c1 * b2 - c2 * b1) / det;
	double y = (a1 * c2 - a2 * c1) / det;
	printf("%gx + %gy = %g\n", a1, b1, c1);
	printf("%gx + %gy = %g\n", a2, b2, c2);
	printf("Solution: x = %g, y = %g\n", x, y);
}

/* main */
void calc() {
	char line[MAX_CMD];

	while (1) {
		printf("[calculator | IN]> ");
		if (!fgets(line, sizeof(line), stdin))
			break;

		rmnl(line);

		if (cmd(line, "exit"))
			break;
		else if (cmd(line, "clear") || cmd(line, "clr") || cmd(line, "cls"))
			clrscr();
		else if (cmd(line, "mode"))
			mode();
		else if (cmd(line, "eqn"))
			eqn();
		else if (!strlen(line))
			continue;
		else {
			double res = te_interp(line, 0);
			printf("[calculator | OUT]> %g\n", res);
		}
	}
}
