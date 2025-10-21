void redraw_messages();
void resize_windows();
void handle_winch(int sig);
void get_usrname_serverip(char *username, int username_size, char *server_ip,
                          int server_ip_size);
int  chat_client();
