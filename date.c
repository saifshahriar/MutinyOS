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

	if (strftime((char[100]) { 0 }, 100, "%a %b %d %H:%M:%S %z %Y\n", local)) {
		char buffer[100];
		strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %z %Y\n", local);
		fputs(buffer, stdout);
	}
}
