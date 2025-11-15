#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "touch.h"

void touch_cmd(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: touch <filename>\n");
		return;
	}

	for (int i = 1; i < argc; i++) {
		int fd = open(argv[i], O_WRONLY | O_CREAT, 0644);
		if (fd < 0) {
			fprintf(stderr, "touch: %s: %s\n", argv[i], strerror(errno));
			continue;
		}
		close(fd);

		struct timespec times[2];
		times[0].tv_nsec = UTIME_NOW;
		times[1].tv_nsec = UTIME_NOW;
		utimensat(AT_FDCWD, argv[i], times, 0);
	}
}
