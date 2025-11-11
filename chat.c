#include <czmq.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

#define PORT         1111
#define BUFFER_SIZE  1024
#define MAX_MESSAGES 1000

WINDOW *msg_win;
WINDOW *input_win;
int     rows, cols;

char *messages[MAX_MESSAGES];
int   msg_count = 0;

static void redraw_messages() {
	werase(msg_win);
	int start = msg_count > rows - 3 ? msg_count - (rows - 3) : 0;
	for (int i = start; i < msg_count; i++)
		wprintw(msg_win, "%s\n", messages[i]);
	box(msg_win, 0, 0);
	wrefresh(msg_win);
}

static void resize_windows() {
	getmaxyx(stdscr, rows, cols);

	wresize(msg_win, rows - 3, cols);
	mvwin(msg_win, 0, 0);
	wresize(input_win, 3, cols);
	mvwin(input_win, rows - 3, 0);

	box(input_win, 0, 0);

	redraw_messages();
	wrefresh(input_win);
}

static void handle_winch(int sig) {
	endwin();
	refresh();
	clear();
	resize_windows();
}

static void get_usrname_serverip(/* char *username, int username_size, */
                                 char *server_ip, int server_ip_size) {
	/* printf("Enter your name: "); */
	/* fgets(username, username_size, stdin); */
	/* username[strcspn(username, "\n")] = 0; */

	printf("Enter server IP (DEFAULT 127.0.0.1): ");
	fgets(server_ip, server_ip_size, stdin);
	server_ip[strcspn(server_ip, "\n")] = 0;

	if (strlen(server_ip) <= 3) {
		strncpy(server_ip, "127.0.0.1", server_ip_size - 1);
		server_ip[server_ip_size - 1] = '\0';
	}
}

int chat_client() {
	char *username = globals.username;
	char  server_ip[64];

	get_usrname_serverip(/* username, sizeof(username),*/ server_ip,
	                     sizeof(server_ip));

	char endpoint[128];
	snprintf(endpoint, sizeof(endpoint), "tcp://%s:%d", server_ip, PORT);

	zsock_t *client = zsock_new_dealer(endpoint);
	if (!client) {
		zsys_error("Failed to connect to server");
		return -1;
	}

	zstr_send(client, username);

	initscr();
	cbreak();
	noecho();
	curs_set(1);
	signal(SIGWINCH, handle_winch);

	getmaxyx(stdscr, rows, cols);

	msg_win   = newwin(rows - 3, cols, 0, 0);
	input_win = newwin(3, cols, rows - 3, 0);

	scrollok(msg_win, TRUE);

	box(msg_win, 0, 0);
	box(input_win, 0, 0);

	wrefresh(msg_win);
	wrefresh(input_win);

	keypad(input_win, TRUE);
	nodelay(input_win, TRUE);

	char input[BUFFER_SIZE] = { 0 };
	int  pos                = 0;

	while (1) {
		zsock_set_rcvtimeo(client, 50);
		char *msg = zstr_recv(client);
		if (msg) {
			if (msg_count < MAX_MESSAGES)
				messages[msg_count++] = strdup(msg);
			redraw_messages();
			zstr_free(&msg);
		}

		int ch = wgetch(input_win);
		if (ch == '\n') {
			input[pos] = '\0';

			/* commands */
			if (!strcmp(input, "/q") || !strcmp(input, "/quit")
			    || !strcmp(input, "/exit")) {
				zstr_sendf(client, "[DISCONNECTED] %s left!", username);
				break;
			} else if (!strcmp(input, "/clear") || !strcmp(input, "/cls")) {
				msg_count = 0;
				resize_windows();
			} else if (pos > 0)
				zstr_sendf(client, "%s: %s", username, input);

			pos = 0;
			memset(input, 0, sizeof(input));
		} else if (ch == KEY_BACKSPACE || ch == 127) {
			if (pos > 0) {
				pos--;
				input[pos] = '\0';
			}
		} else if (ch >= 32 && ch <= 126 && pos < BUFFER_SIZE - 1) {
			input[pos++] = ch;
			input[pos]   = '\0';
		}

		werase(input_win);
		box(input_win, 0, 0);
		mvwprintw(input_win, 1, 1, "> %s", input);
		wmove(input_win, 1, 3 + pos);
		wrefresh(input_win);
	}

	for (int i = 0; i < msg_count; i++)
		free(messages[i]);
	endwin();
	zsock_destroy(&client);
	return 0;
}
