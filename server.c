#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PORT        1111
#define MAX_HISTORY 1000

int main() {
	char endpoint[64];
	snprintf(endpoint, sizeof(endpoint), "tcp://*:%d", PORT);

	zsock_t *server = zsock_new_router(endpoint);
	if (!server) {
		zsys_error("Failed to bind server socket");
		return -1;
	}

	printf("CZMQ group chat server started on %s\n", endpoint);

	zlist_t *clients = zlist_new();

	char *history[MAX_HISTORY];
	int   history_count = 0;

	while (true) {
		zmsg_t *msg = zmsg_recv(server);
		if (!msg)
			break;

		zframe_t *identity = zmsg_pop(msg);

		char *text = zmsg_popstr(msg);
		zmsg_destroy(&msg);

		int known = 0;
		for (void *item = zlist_first(clients); item != NULL;
		     item       = zlist_next(clients)) {
			if (zframe_eq(identity, (zframe_t *)item)) {
				known = 1;
				break;
			}
		}

		if (!known) {
			zlist_append(clients, zframe_dup(identity));

			for (int i = 0; i < history_count; i++) {
				zmsg_t   *out = zmsg_new();
				zframe_t *dup = zframe_dup(identity);
				zmsg_append(out, &dup);
				zmsg_addstr(out, history[i]);
				zmsg_send(&out, server);
			}
		}

		time_t    t  = time(NULL);
		struct tm tm = *localtime(&t);
		char      broadcast[1024];
		snprintf(broadcast, sizeof(broadcast), " [%02d:%02d:%02d] %s%s%s",
		         tm.tm_hour, tm.tm_min, tm.tm_sec, !known ? "[CONNECTED] " : "",
		         text, !known ? " joined the chat!" : "");

		printf("%s\n", broadcast);

		if (history_count < MAX_HISTORY)
			history[history_count++] = strdup(broadcast);

		for (void *item = zlist_first(clients); item != NULL;
		     item       = zlist_next(clients)) {
			zframe_t *client_id = (zframe_t *)item;

			zmsg_t   *out = zmsg_new();
			zframe_t *dup = zframe_dup(client_id);
			zmsg_append(out, &dup);
			zmsg_addstr(out, broadcast);
			zmsg_send(&out, server);
		}

		zstr_free(&text);
		zframe_destroy(&identity);
	}

	for (void *item = zlist_first(clients); item != NULL;
	     item       = zlist_next(clients)) {
		zframe_destroy((zframe_t *)item);
	}
	zlist_destroy(&clients);

	for (int i = 0; i < history_count; i++)
		free(history[i]);

	zsock_destroy(&server);
	return 0;
}
