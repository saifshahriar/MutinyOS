// compatibility.h

#ifdef _WIN32
	#include <windows.h>
#else
	#include <curses.h>
	#include <term.h>
#endif

#include "compatibility.h"

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
