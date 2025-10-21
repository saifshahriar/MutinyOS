#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, 1);
	nodelay(stdscr, 1);

	int posX  = 1;
	int posY  = 1;
	int foodX = rand() % 18 + 1;
	int foodY = rand() % 18 + 1;
	int dirX  = 1;
	int dirY  = 0;

	while (1) {
		int pressed = getch();
		switch (pressed) {
			case 'h':
			case KEY_LEFT:
				dirX = -1;
				dirY = 0;
				break;
			case 'l':
			case KEY_RIGHT:
				dirX = 1;
				dirY = 0;
				break;
			case 'k':
			case KEY_UP:
				dirX = 0;
				dirY = -1;
				break;
			case 'j':
			case KEY_DOWN:
				dirX = 0;
				dirY = 1;
				break;
			case 'q':
				endwin();
				return EXIT_SUCCESS;
		}

		int rows, cols;
		getmaxyx(stdscr, rows, cols);

		if (posX == -1)
			posX = cols;
		if (posY == -1)
			posY = rows;

		posX += dirX;
		posY += dirY;



		clear();
		box(stdscr, 0, 0);             // draw border
		mvaddstr(posY, posX, "*");     // draw snake
		mvaddstr(foodY, foodX, "@");   // draw food
		refresh();

		usleep(100000);
	}
}
