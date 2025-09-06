// compatibility.h
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef enum {
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
	DEFAULT
} Color;

void clrscr(void);
void color(Color c, const char *text);
/* char *colorize(Color c, const char *text); */
