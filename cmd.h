typedef struct {
	char *cmd;
	char *alias;
	char *desc;
} Prog;

int  cmd(char *inp, char *cmd);
int  cnt_cmd();
int  cnt_prog();
void print_cmds();
void print_progs();
