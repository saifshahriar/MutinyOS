#ifndef GLOBAL_H
#define GLOBAL_H

#define BUFFSIZE 256

typedef struct {
	char username[BUFFSIZE];
	char hostname[BUFFSIZE];
	char fullname[BUFFSIZE];
	char password[BUFFSIZE];
	char homedir[BUFFSIZE * 2];
} Global;

extern Global      globals;
extern const char *BASE_DIR_PATH;
extern const char *HOST_F;
extern const char *SALT;
extern const char *SETUP_F;
extern const char *SHADOWS_F;
extern const char *USERS_F;

#endif   // GLOBAL_H
