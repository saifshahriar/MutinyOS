#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rm.h"

static int remove_recursively(const char *path) {
	struct stat st;
	if (lstat(path, &st)) {
		fprintf(stderr, "rm: %s: %s\n", path, strerror(errno));
		return -1;
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dp = opendir(path);
		if (!dp) {
			fprintf(stderr, "rm: %s: %s\n", path, strerror(errno));
			return -1;
		}

		struct dirent *d;
		while ((d = readdir(dp))) {
			if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))
				continue;

			char child[4096];
			snprintf(child, sizeof(child), "%s/%s", path, d->d_name);
			remove_recursively(child);
		}
		closedir(dp);

		if (rmdir(path)) {
			fprintf(stderr, "rm: %s: %s\n", path, strerror(errno));
			return -1;
		}
	} else if (unlink(path)) {
		fprintf(stderr, "rm: %s: %s\n", path, strerror(errno));
		return -1;
	}

	return 0;
}

void rm_cmd(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: rm [-r] <file/dir> [file/dir ...]\n");
		return;
	}

	int recursive = 0;
	int start     = 1;

	if (strcmp(argv[1], "-r") == 0) {
		recursive = 1;
		start     = 2;
		if (argc < 3) {
			printf("Usage: rm [-r] <file/dir> [file/dir ...]\n");
			return;
		}
	}

	for (int i = start; i < argc; i++) {
		if (recursive)
			remove_recursively(argv[i]);
		else {
			struct stat st;
			if (!stat(argv[i], &st) && S_ISDIR(st.st_mode))
				fprintf(stderr, "rm: %s: is a directory (use -r)\n", argv[i]);
			else if (unlink(argv[i]))
				fprintf(stderr, "rm: %s: %s\n", argv[i], strerror(errno));
		}
	}
}
