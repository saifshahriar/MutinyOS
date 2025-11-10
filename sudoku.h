void init_colors();
int is_valid();
int solve_sudoku(int grid[9][9]);
void fill_diagonal_boxes(int grid[9][9]);
void generate_full_sudoku(int grid[9][9]);
void remove_numbers(int grid[9][9], int count);
void generate_puzzle(int difficulty);
void draw_sudoku_play();
int check_solution();
void give_hint();
void play_sudoku();
void select_difficulty();


