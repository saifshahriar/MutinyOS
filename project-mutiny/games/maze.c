#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH  21
#define HEIGHT 21

char maze[HEIGHT][WIDTH];

int dx[] = { 0, 0, -2, 2 };
int dy[] = { -2, 2, 0, 0 };

void init_maze() {
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			maze[y][x] = '#';
}

int is_valid(int y, int x) {
	return x > 0 && x < WIDTH - 1 && y > 0 && y < HEIGHT - 1;
}

void carve_maze(int y, int x) {
	maze[y][x] = ' ';
	int dirs[] = { 0, 1, 2, 3 };
	for (int i = 3; i > 0; i--) {
		int j   = rand() % (i + 1);
		int tmp = dirs[i];
		dirs[i] = dirs[j];
		dirs[j] = tmp;
	}

	for (int i = 0; i < 4; i++) {
		int nx = x + dx[dirs[i]];
		int ny = y + dy[dirs[i]];
		if (is_valid(ny, nx) && maze[ny][nx] == '#') {
			maze[y + dy[dirs[i]] / 2][x + dx[dirs[i]] / 2] = ' ';
			carve_maze(ny, nx);
		}
	}
}

chtype wall_char(int y, int x) {
	if (maze[y][x] != '#')
		return ' ';
	int up    = (y > 0 && maze[y - 1][x] == '#');
	int down  = (y < HEIGHT - 1 && maze[y + 1][x] == '#');
	int left  = (x > 0 && maze[y][x - 1] == '#');
	int right = (x < WIDTH - 1 && maze[y][x + 1] == '#');

	int count = up + down + left + right;

	if (count >= 3)
		return ACS_PLUS;
	if ((up || down) && !(left || right))
		return ACS_VLINE;
	if ((left || right) && !(up || down))
		return ACS_HLINE;
	if ((up || down) && (left || right))
		return ACS_PLUS;
	return ACS_CKBOARD;
}

void draw_maze(WINDOW* win) {
	werase(win);
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			mvwaddch(win, y, x, wall_char(y, x));

	mvwaddch(win, 0, 0, ACS_ULCORNER);
	mvwaddch(win, 0, WIDTH - 1, ACS_URCORNER);
	mvwaddch(win, HEIGHT - 1, 0, ACS_LLCORNER);
	mvwaddch(win, HEIGHT - 1, WIDTH - 1, ACS_LRCORNER);
	for (int x = 1; x < WIDTH - 1; x++) {
		mvwaddch(win, 0, x, ACS_HLINE);
		mvwaddch(win, HEIGHT - 1, x, ACS_HLINE);
	}
	for (int y = 1; y < HEIGHT - 1; y++) {
		mvwaddch(win, y, 0, ACS_VLINE);
		mvwaddch(win, y, WIDTH - 1, ACS_VLINE);
	}

	wrefresh(win);
}

int main() {
	srand(time(NULL));
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, 1);
	nodelay(stdscr, 1);

	WINDOW* win = newwin(HEIGHT, WIDTH, 0, 0);

	int playerX = 1, playerY = 1;
	int goalX = WIDTH - 2, goalY = HEIGHT - 2;

	init_maze();
	carve_maze(1, 1);
	maze[playerY][playerX] = ' ';
	maze[goalY][goalX]     = ' ';

	while (1) {
		int pressed = getch();
		int nextX = playerX, nextY = playerY;

		switch (pressed) {
			case 'h':
			case KEY_LEFT:
				nextX--;
				break;
			case 'l':
			case KEY_RIGHT:
				nextX++;
				break;
			case 'k':
			case KEY_UP:
				nextY--;
				break;
			case 'j':
			case KEY_DOWN:
				nextY++;
				break;
			case 'q':
				endwin();
				return 0;
		}

		if (maze[nextY][nextX] == ' ') {
			playerX = nextX;
			playerY = nextY;
		}

		draw_maze(win);
		mvwaddch(win, playerY, playerX, '*');   // player
		mvwaddch(win, goalY, goalX, '@');       // goal
		wrefresh(win);

		if (playerX == goalX && playerY == goalY) {
			mvprintw(HEIGHT, 0, "You reached the goal! Press q to quit.");
			nodelay(stdscr, 0);
		}

		usleep(100000);
	}

	endwin();
	return 0;
}
