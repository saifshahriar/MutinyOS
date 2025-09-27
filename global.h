#define BUFFSIZE 256

typedef struct {
	char username[BUFFSIZE];
	char hostname[BUFFSIZE];
} Global;

extern Global globals;
