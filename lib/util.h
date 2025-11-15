/* util.h */
#ifndef UTIL_H
#define UTIL_H

int  isfempty(const char *filename);
void rmnl(char *line);
int  tokenize(char *line, char *argv[], int max_args);
#endif
