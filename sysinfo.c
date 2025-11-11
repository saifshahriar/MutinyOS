#include <ctype.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#define MAX_CPUS 64
int num_cpus = 0;

#include "sysinfo.h"

unsigned long long prev_idle[MAX_CPUS], prev_total[MAX_CPUS];

static int read_cpu_line(const char *line, unsigned long long *idle,
                         unsigned long long *total) {
	unsigned long long user, nice, system, iowait, irq, softirq, steal;
	int n = sscanf(line, "cpu%*d %llu %llu %llu %llu %llu %llu %llu", &user,
	               &nice, &system, &iowait, &irq, &softirq, &steal);
	if (n < 7)
		return 0;
	*idle  = iowait;
	*total = user + nice + system + iowait + irq + softirq + steal;
	return 1;
}

static int read_all_cpu(double cpu_percent[MAX_CPUS]) {
	FILE *f = fopen("/proc/stat", "r");
	if (!f)
		return 0;
	char buf[256];
	int  idx = 0;
	while (fgets(buf, sizeof(buf), f)) {
		if (strncmp(buf, "cpu", 3) != 0)
			break;
		if (idx > 0) {
			unsigned long long idle, total;
			if (read_cpu_line(buf, &idle, &total)) {
				unsigned long long d_total = total - prev_total[idx - 1];
				unsigned long long d_idle  = idle - prev_idle[idx - 1];
				cpu_percent[idx - 1] =
				    d_total ? (double)(d_total - d_idle) / d_total * 100.0 : 0;
				prev_idle[idx - 1]  = idle;
				prev_total[idx - 1] = total;
			}
		}
		idx++;
	}
	fclose(f);
	return idx - 1;
}

static void read_mem(double *used, double *total) {
	FILE *f = fopen("/proc/meminfo", "r");
	if (!f) {
		*used = *total = 0;
		return;
	}
	unsigned long long memtotal = 0, memavail = 0;
	char               line[128];
	while (fgets(line, sizeof(line), f)) {
		if (sscanf(line, "MemTotal: %llu kB", &memtotal) == 1)
			continue;
		if (sscanf(line, "MemAvailable: %llu kB", &memavail) == 1)
			continue;
	}
	fclose(f);
	*total = memtotal / 1024.0;
	*used  = (memtotal - memavail) / 1024.0;
}

static void read_load(double loads[3]) {
	FILE *f = fopen("/proc/loadavg", "r");
	if (f) {
		fscanf(f, "%lf %lf %lf", &loads[0], &loads[1], &loads[2]);
		fclose(f);
	} else
		loads[0] = loads[1] = loads[2] = 0;
}

static double read_battery() {
	FILE *f = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	if (!f)
		return -1;
	int val;
	fscanf(f, "%d", &val);
	fclose(f);
	return val;
}

static void read_disk(const char *path, double *used, double *total) {
	struct statvfs st;
	if (statvfs(path, &st) != 0) {
		*used = *total = 0;
		return;
	}
	*total = st.f_blocks * st.f_frsize / 1024.0 / 1024.0;
	*used  = (*total - st.f_bfree * st.f_frsize / 1024.0 / 1024.0);
}

static void draw_bar(int y, int x, int w, double percent, int color) {
	int filled = (int)(w * percent / 100.0);
	attron(COLOR_PAIR(color));
	for (int i = 0; i < filled; i++)
		mvaddch(y, x + i, '#');
	attroff(COLOR_PAIR(color));
	for (int i = filled; i < w; i++)
		mvaddch(y, x + i, '-');
}

static int detect_cpus() {
	FILE *f = fopen("/proc/stat", "r");
	if (!f)
		return 1;
	char buf[256];
	int  count = 0;
	while (fgets(buf, sizeof(buf), f)) {
		if (strncmp(buf, "cpu", 3) != 0)
			break;
		if (isdigit(buf[3]))
			count++;
	}
	fclose(f);
	return count ? count : 1;
}

int sysinfo() {
	initscr();
	noecho();
	cbreak();
	curs_set(0);
	keypad(stdscr, TRUE);
	if (!has_colors()) {
		endwin();
		printf("No color\n");
		return 1;
	}
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

	num_cpus = detect_cpus();
	memset(prev_idle, 0, sizeof(prev_idle));
	memset(prev_total, 0, sizeof(prev_total));
	double cpu_percent[MAX_CPUS];

	FILE *f = fopen("/proc/stat", "r");
	char  buf[256];
	int   idx = 0;
	while (fgets(buf, sizeof(buf), f) && idx < num_cpus) {
		if (strncmp(buf, "cpu", 3) == 0 && isdigit(buf[3]))
			read_cpu_line(buf, &prev_idle[idx], &prev_total[idx]);
		idx++;
	}
	fclose(f);

	nodelay(stdscr, TRUE);
	while (1) {
		int ch = getch();
		if (ch == 'q' || ch == 'Q')
			break;
		clear();
		mvprintw(0, 2, "Fancy System Monitor - q to quit");

		read_all_cpu(cpu_percent);
		mvprintw(2, 2, "CPU Usage per core:");
		for (int i = 0; i < num_cpus; i++) {
			mvprintw(3 + i, 4, "CPU%-2d: %5.1f %% ", i, cpu_percent[i]);
			int color = cpu_percent[i] < 50 ? 1 : (cpu_percent[i] < 80 ? 3 : 4);
			draw_bar(3 + i, 18, 50, cpu_percent[i], color);
		}

		int row = 3 + num_cpus + 1;

		double ram_used, ram_total;
		read_mem(&ram_used, &ram_total);
		mvprintw(row, 2, "RAM Usage: %.1f / %.1f MB (%.1f%%)", ram_used,
		         ram_total, ram_used / ram_total * 100.0);
		draw_bar(row + 1, 2, 50, ram_used / ram_total * 100.0, 2);
		row += 3;
		double loads[3];
		read_load(loads);
		mvprintw(row, 2, "Load Avg: %.2f %.2f %.2f", loads[0], loads[1],
		         loads[2]);
		row += 2;

		double batt = read_battery();
		if (batt >= 0) {
			mvprintw(row, 2, "Battery: %.0f %%", batt);
			int color = batt > 50 ? 1 : (batt > 20 ? 3 : 4);
			draw_bar(row + 1, 2, 50, batt, color);
			row += 3;
		}

		double dused, dtotal;
		read_disk("/", &dused, &dtotal);
		mvprintw(row, 2, "Disk / : %.1f / %.1f MB (%.1f%%)", dused, dtotal,
		         dused / dtotal * 100.0);
		draw_bar(row + 1, 2, 50, dused / dtotal * 100.0, 5);
		row += 3;
		read_disk("/home", &dused, &dtotal);
		mvprintw(row, 2, "Disk /home : %.1f / %.1f MB (%.1f%%)", dused, dtotal,
		         dused / dtotal * 100.0);
		draw_bar(row + 1, 2, 50, dused / dtotal * 100.0, 5);

		refresh();
		sleep(1);
	}

	endwin();
	return 0;
}
