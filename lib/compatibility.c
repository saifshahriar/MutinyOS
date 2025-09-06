// compatibility.c

#ifdef _WIN32
#include <windows.h>
#else
#include <curses.h>
#include <term.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "compatibility.h"

#ifdef _WIN32
// Map enum to Windows Console colors
WORD get_windows_color(Color c) {
	switch (c) {
	case BLACK:
		return 0;
	case RED:
		return FOREGROUND_RED | FOREGROUND_INTENSITY;
	case GREEN:
		return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case YELLOW:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case BLUE:
		return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case MAGENTA:
		return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case CYAN:
		return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	case WHITE:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	case DEFAULT:
	default:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
}
#endif

void clrscr() {
#ifdef _WIN32
	system("cls");
	/* HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); */
	/*    CONSOLE_SCREEN_BUFFER_INFO csbi; */
	/*    DWORD count, cellCount; */
	/*    COORD homeCoords = {0, 0}; */
	/**/
	/*    if (!GetConsoleScreenBufferInfo(h, &csbi)) return; */
	/*    cellCount = csbi.dwSize.X * csbi.dwSize.Y; */
	/**/
	/*    FillConsoleOutputCharacter(h, ' ', cellCount, homeCoords, &count); */
	/*    FillConsoleOutputAttribute(h, csbi.wAttributes, cellCount, homeCoords,
	 * &count); */
	/*    SetConsoleCursorPosition(h, homeCoords); */
#else
	setupterm(NULL, fileno(stdout), NULL);
	putp(tigetstr("clear"));
#endif
}

void color(Color c, const char *text) {
#ifdef _WIN32
	HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD                       saved_attributes;

	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	SetConsoleTextAttribute(hConsole, get_windows_color(color));
	printf("%s", text);

	SetConsoleTextAttribute(hConsole, saved_attributes);
#else
	// ANSI color codes from 30 to 37 for foreground
	int ac;
	switch (c) {
	case BLACK:
		ac = 30;
		break;
	case RED:
		ac = 31;
		break;
	case GREEN:
		ac = 32;
		break;
	case YELLOW:
		ac = 33;
		break;
	case BLUE:
		ac = 34;
		break;
	case MAGENTA:
		ac = 35;
		break;
	case CYAN:
		ac = 36;
		break;
	case WHITE:
		ac = 37;
		break;
	case DEFAULT:
	default:
		ac = 39;
		break;   // default foreground
	}
	printf("\033[%dm%s\033[0m", ac, text);
#endif
}

/* char *colorize(Color c, const char *text) { */
/* 	int ac; */
/* 	switch (c) { */
/* 	case BLACK: */
/* 		ac = 30; */
/* 		break; */
/* 	case RED: */
/* 		ac = 31; */
/* 		break; */
/* 	case GREEN: */
/* 		ac = 32; */
/* 		break; */
/* 	case YELLOW: */
/* 		ac = 33; */
/* 		break; */
/* 	case BLUE: */
/* 		ac = 34; */
/* 		break; */
/* 	case MAGENTA: */
/* 		ac = 35; */
/* 		break; */
/* 	case CYAN: */
/* 		ac = 36; */
/* 		break; */
/* 	case WHITE: */
/* 		ac = 37; */
/* 		break; */
/* 	case DEFAULT: */
/* 	default: */
/* 		ac = 39; */
/* 		break; */
/* 	} */
/**/
/* 	size_t len = snprintf(NULL, 0, "\033[%dm%s\033[0m", ac, text) + 1; */
/* 	char  *buf = malloc(len);   // always free this alloc here, comrades */
/* 	if (!buf) */
/* 		return NULL; */
/* 	snprintf(buf, len, "\033[%dm%s\033[0m", ac, text); */
/* 	return buf; */
/* } */
