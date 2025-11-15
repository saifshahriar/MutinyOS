#include <dirent.h>
#include <stdio.h>

#include "ls.h"

void ls() {
	DIR           *d = opendir(".");
	struct dirent *de;

	if (!d)
		return;

	while ((de = readdir(d)) != NULL)
		printf("%s\n", de->d_name);

	closedir(d);
}
