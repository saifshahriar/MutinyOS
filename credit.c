#include <stdint.h>
#include <stdio.h>

#include "credit.h"

void credit() {
	uint32_t       n = 0x53414946;   // 'S' 'A' 'I' 'F' in little-endian form
	unsigned char *p = (unsigned char *)&n;

	printf("Created by ");
	for (int i = sizeof(n) - 1; i >= 0; --i)
		putchar(p[i]);
	putchar('\n');
}
