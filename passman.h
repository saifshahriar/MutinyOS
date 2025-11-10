void draw_box(WINDOW *w, int color_pair);
void print_center(WINDOW *w, int y, int width, const char *s, int color_pair);
void get_password_masked(WINDOW *w, int y, int x, char *buffer, int max_len);
void set_master_password();
int check_master_prompt();
void add_entry();
void view_entries();
void delete_entry();

