#include <stddef.h>

#define MAX_ARGS 20
#define MAX_CMD  100

typedef enum {
	SIMUL2,
	SIMUL3,
	SIMUL4,
	POLY2,
	POLY3,
	POLY4,
} EqnType;

typedef struct {
	char *name;
	char *desc;
} Mode;

void calc();
void eqn();
void help();
void mode();
void solve_eqn2x2(double a1, double b1, double c1, double a2, double b2,
                  double c2);
