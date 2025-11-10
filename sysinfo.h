int read_cpu_line(const char *line, unsigned long long *idle, unsigned long long *total) ;
int read_all_cpu(double cpu_percent[MAX_CPUS]);
void read_mem(double *used, double *total);
void read_load(double loads[3]) ;
double read_battery() ;
void read_disk(const char *path, double *used, double *total);
void draw_bar(int y,int x,int w,double percent,int color);
int detect_cpus() ;
