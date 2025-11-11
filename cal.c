#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static const char *s_mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char *s_day[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };

static int isleap(int y) {
	return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}

static int monthlength(int y, int m) {
	int len[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	return (m == 2 && isleap(y)) ? 29 : len[m - 1];
}

static void p_mon(int y, int m, int curr_date) {
	struct tm t = { 0 };
	time_t    tm;
	int       fd_week, ld_week, curr_day, ccnt;
	char      buf[20];

	t.tm_year  = y - 1900;
	t.tm_mon   = m - 1;
	t.tm_mday  = 1;
	t.tm_isdst = -1;

	tm = mktime(&t);
	if (tm == (time_t)-1)
		return;

	fd_week = t.tm_wday;
	ld_week = monthlength(y, m);

	snprintf(buf, sizeof(buf), "%s %d", s_mon[m - 1], y);
	ccnt = 20 - strlen(buf);
	printf("%*s%s%*s\n", ccnt / 2 + ccnt % 2, "", buf, ccnt / 2, "");

	for (curr_day = 0; curr_day < 7; ++curr_day)
		printf("%s ", s_day[curr_day]);
	printf("\n");

	for (ccnt = 0; ccnt < fd_week; ++ccnt)
		printf("   ");

	curr_day = 1;
	ccnt     = fd_week;

	while (curr_day <= ld_week) {
		if (ccnt == 7) {
			printf("\n");
			ccnt = 0;
		}

		if (curr_day == curr_date)
			printf("\x1b[7m%2d\x1b[0m ", curr_day);
		else
			printf("%2d ", curr_day);

		curr_day++;
		ccnt++;
	}

	if (ccnt > 0)
		printf("\n");
}

int cal(void) {
	time_t     now = time(NULL);
	struct tm *lt  = localtime(&now);
	if (!lt)
		return 1;

	int y = lt->tm_year + 1900;
	int m = lt->tm_mon + 1;
	int d = lt->tm_mday;

	p_mon(y, m, d);
	return 0;
}
