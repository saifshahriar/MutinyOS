#include <stdio.h>
#include <time.h>

void date() {
	time_t     t;
	struct tm *local;

	t = time(NULL);

	if (t == ((time_t)-1)) {
		perror("time");
		return;
	}

	local = localtime(&t);
	if (local == NULL) {
		perror("localtime");
		return;
	}

	char buf[100];
	if (strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %z %Y\n", local))
		fputs(buf, stdout);
}
