#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"
int puzzle[9][9];
int solution[9][9];
int original[9][9];
int cursor_row = 0;
int cursor_col = 0;

void init_colors() {
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_CYAN);
}

int is_valid(int grid[9][9], int row, int col, int num) {
    for (int x = 0; x < 9; x++) {
        if (grid[row][x] == num || grid[x][col] == num) {
            return 0;
        }
    }
    
    int start_row = row - row % 3;
    int start_col = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + start_row][j + start_col] == num) {
                return 0;
            }
        }
    }
    
    return 1;
}

int solve_sudoku(int grid[9][9]) {
    int row, col;
    int is_empty = 0;
    
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (grid[row][col] == 0) {
                is_empty = 1;
                break;
            }
        }
        if (is_empty) break;
    }
    
    if (!is_empty) return 1;
    
    for (int num = 1; num <= 9; num++) {
        if (is_valid(grid, row, col, num)) {
            grid[row][col] = num;
            
            if (solve_sudoku(grid)) {
                return 1;
            }
            
            grid[row][col] = 0;
        }
    }
    
    return 0;
}

void fill_diagonal_boxes(int grid[9][9]) {
    for (int box = 0; box < 9; box += 3) {
        int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        
        for (int i = 8; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = nums[i];
            nums[i] = nums[j];
            nums[j] = temp;
        }
        
        int idx = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                grid[box + i][box + j] = nums[idx++];
            }
        }
    }
}

void generate_full_sudoku(int grid[9][9]) {
    memset(grid, 0, sizeof(int) * 81);
    fill_diagonal_boxes(grid);
    solve_sudoku(grid);
}

void remove_numbers(int grid[9][9], int count) {
    int removed = 0;
    while (removed < count) {
        int row = rand() % 9;
        int col = rand() % 9;
        
        if (grid[row][col] != 0) {
            grid[row][col] = 0;
            removed++;
        }
    }
}

void generate_puzzle(int difficulty) {
    int remove_count;
    switch (difficulty) {
        case 1: remove_count = 30; break;
        case 2: remove_count = 45; break;
        case 3: remove_count = 55; break;
        default: remove_count = 45;
    }
    
    generate_full_sudoku(solution);
    memcpy(puzzle, solution, sizeof(puzzle));
    remove_numbers(puzzle, remove_count);
    memcpy(original, puzzle, sizeof(original));
}

void draw_sudoku_play() {
    clear();
    
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(1, (COLS - 12) / 2, "SUDOKU GAME");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    int start_row = 4;
    int start_col = (COLS - 37) / 2;
    
    attron(COLOR_PAIR(1));
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int row = start_row + i * 2;
            int col = start_col + j * 4;
            
            if (i % 3 == 0) {
                attron(A_BOLD);
                mvprintw(row, col, "+---");
                attroff(A_BOLD);
            } else {
                mvprintw(row, col, "+---");
            }
            
            if (j % 3 == 0) {
                attron(A_BOLD);
                mvprintw(row + 1, col, "|");
                attroff(A_BOLD);
            } else {
                mvprintw(row + 1, col, "|");
            }
        }
        if (i % 3 == 0) {
            attron(A_BOLD);
            mvprintw(start_row + i * 2, start_col + 36, "+");
            attroff(A_BOLD);
        } else {
            mvprintw(start_row + i * 2, start_col + 36, "+");
        }
        
        mvprintw(start_row + i * 2 + 1, start_col + 36, "|");
    }
    
    attron(A_BOLD);
    for (int j = 0; j < 9; j++) {
        mvprintw(start_row + 18, start_col + j * 4, "+---");
    }
    mvprintw(start_row + 18, start_col + 36, "+");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int row = start_row + i * 2 + 1;
            int col = start_col + j * 4 + 2;
            
            if (i == cursor_row && j == cursor_col && original[i][j] == 0) {
                attron(COLOR_PAIR(7) | A_BOLD);
                mvprintw(row, col, "%c", puzzle[i][j] == 0 ? ' ' : puzzle[i][j] + '0');
                attroff(COLOR_PAIR(7) | A_BOLD);
            } else if (puzzle[i][j] != 0) {
                if (original[i][j] != 0) {
                    attron(COLOR_PAIR(3) | A_BOLD);
                } else {
                    if (puzzle[i][j] == solution[i][j]) {
                        attron(COLOR_PAIR(2));
                    } else {
                        attron(COLOR_PAIR(4));
                    }
                }
                mvprintw(row, col, "%d", puzzle[i][j]);
                if (original[i][j] != 0) {
                    attroff(COLOR_PAIR(3) | A_BOLD);
                } else {
                    if (puzzle[i][j] == solution[i][j]) {
                        attroff(COLOR_PAIR(2));
                    } else {
                        attroff(COLOR_PAIR(4));
                    }
                }
            } else {
                attron(COLOR_PAIR(6));
                mvprintw(row, col, " ");
                attroff(COLOR_PAIR(6));
            }
        }
    }
    
    attron(COLOR_PAIR(5));
    mvprintw(start_row + 20, start_col - 5, "Arrow Keys: Move | 1-9: Enter Number");
    mvprintw(start_row + 21, start_col - 5, "0/Delete: Clear | C: Check Solution");
    mvprintw(start_row + 22, start_col - 5, "H: Hint | S: Show Solution | Q: Quit");
    attroff(COLOR_PAIR(5));
    
    refresh();
}

int check_solution() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] != solution[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

void give_hint() {
    int empty_cells[81][2];
    int count = 0;
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] == 0 && original[i][j] == 0) {
                empty_cells[count][0] = i;
                empty_cells[count][1] = j;
                count++;
            }
        }
    }
    
    if (count > 0) {
        int idx = rand() % count;
        int row = empty_cells[idx][0];
        int col = empty_cells[idx][1];
        puzzle[row][col] = solution[row][col];
        cursor_row = row;
        cursor_col = col;
    }
}

void play_sudoku() {
    cursor_row = 0;
    cursor_col = 0;
    
    while (1) {
        draw_sudoku_play();
        
        int ch = getch();
        
        switch (ch) {
            case KEY_UP:
                if (cursor_row > 0) cursor_row--;
                break;
            case KEY_DOWN:
                if (cursor_row < 8) cursor_row++;
                break;
            case KEY_LEFT:
                if (cursor_col > 0) cursor_col--;
                break;
            case KEY_RIGHT:
                if (cursor_col < 8) cursor_col++;
                break;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                if (original[cursor_row][cursor_col] == 0) {
                    puzzle[cursor_row][cursor_col] = ch - '0';
                }
                break;
            case '0':
            case KEY_DC:
            case KEY_BACKSPACE:
            case 127:
                if (original[cursor_row][cursor_col] == 0) {
                    puzzle[cursor_row][cursor_col] = 0;
                }
                break;
            case 'c':
            case 'C':
                if (check_solution()) {
                    attron(COLOR_PAIR(2) | A_BOLD);
                    mvprintw(LINES - 2, (COLS - 30) / 2, "CONGRATULATIONS! You solved it!");
                    attroff(COLOR_PAIR(2) | A_BOLD);
                } else {
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES - 2, (COLS - 40) / 2, "Not quite right yet. Keep trying!");
                    attroff(COLOR_PAIR(4));
                }
                refresh();
                getch();
                break;
            case 'h':
            case 'H':
                give_hint();
                break;
            case 's':
            case 'S':
                memcpy(puzzle, solution, sizeof(puzzle));
                break;
            case 'q':
            case 'Q':
                return;
        }
    }
}

void select_difficulty() {
    clear();
    
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(2, (COLS - 18) / 2, "SELECT DIFFICULTY");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    attron(COLOR_PAIR(3));
    mvprintw(5, (COLS - 30) / 2, "1. Easy (30 cells removed)");
    mvprintw(6, (COLS - 30) / 2, "2. Medium (45 cells removed)");
    mvprintw(7, (COLS - 30) / 2, "3. Hard (55 cells removed)");
    attroff(COLOR_PAIR(3));
    
    attron(COLOR_PAIR(5));
    mvprintw(9, (COLS - 25) / 2, "Enter choice (1-3): ");
    attroff(COLOR_PAIR(5));
    refresh();
    
    int difficulty;
    scanw("%d", &difficulty);
    
    if (difficulty >= 1 && difficulty <= 3) {
        attron(COLOR_PAIR(2));
        mvprintw(11, (COLS - 25) / 2, "Generating puzzle...");
        attroff(COLOR_PAIR(2));
        refresh();
        
        generate_puzzle(difficulty);
        play_sudoku();
    }
}

int main() {
    srand(time(NULL));
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (has_colors()) {
        init_colors();
    }
    
    while (1) {
        clear();
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(2, (COLS - 14) / 2, "SUDOKU GAME");
        attroff(COLOR_PAIR(2) | A_BOLD);
        
        attron(COLOR_PAIR(3));
        mvprintw(5, (COLS - 20) / 2, "1. New Game");
        mvprintw(6, (COLS - 20) / 2, "2. Exit");
        attroff(COLOR_PAIR(3));
        
        attron(COLOR_PAIR(5));
        mvprintw(8, (COLS - 20) / 2, "Enter choice: ");
        attroff(COLOR_PAIR(5));
        refresh();
        
        int choice;
        scanw("%d", &choice);
        
        switch (choice) {
            case 1:
                select_difficulty();
                break;
            case 2:
                endwin();
                return 0;
            default:
                attron(COLOR_PAIR(4));
                mvprintw(LINES - 2, (COLS - 20) / 2, "Invalid choice!");
                attroff(COLOR_PAIR(4));
                getch();
        }
    }
    
    endwin();
    return 0;
}