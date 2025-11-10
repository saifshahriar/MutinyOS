/* snake_game.c - colorful snake game using ncurses */
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Color definitions
#define COLOR_SNAKE_HEAD 1
#define COLOR_SNAKE_BODY 2
#define COLOR_FOOD 3
#define COLOR_WALL 4
#define COLOR_SCORE 5
#define COLOR_GAME_OVER 6

// Game constants
#define GAME_WIDTH 60
#define GAME_HEIGHT 20
#define INITIAL_SNAKE_LENGTH 3
#define GAME_SPEED 100000  // microseconds

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position *body;
    int length;
    int direction;  // 0: up, 1: right, 2: down, 3: left
    int max_length;
} Snake;

typedef struct {
    Position pos;
    int active;
} Food;

// Game state
Snake snake;
Food food;
int score;
int game_over;
int game_paused;

void init_colors() {
    start_color();
    init_pair(COLOR_SNAKE_HEAD, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_SNAKE_BODY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_FOOD, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_WALL, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_SCORE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_GAME_OVER, COLOR_RED, COLOR_BLACK);
}

void init_game() {
    // Initialize snake
    snake.max_length = GAME_WIDTH * GAME_HEIGHT;
    snake.body = malloc(snake.max_length * sizeof(Position));
    snake.length = INITIAL_SNAKE_LENGTH;
    snake.direction = 1;  // Start moving right
    
    // Initialize snake body in the middle
    int start_x = GAME_WIDTH / 2;
    int start_y = GAME_HEIGHT / 2;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = start_x - i;
        snake.body[i].y = start_y;
    }
    
    // Initialize food
    food.active = 0;
    
    // Game state
    score = 0;
    game_over = 0;
    game_paused = 0;
    
    srand(time(NULL));
}

void cleanup_game() {
    free(snake.body);
}

void generate_food() {
    if (food.active) return;
    
    int valid_position;
    do {
        valid_position = 1;
        food.pos.x = rand() % (GAME_WIDTH - 2) + 1;
        food.pos.y = rand() % (GAME_HEIGHT - 2) + 1;
        
        // Check if food overlaps with snake
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.pos.x && snake.body[i].y == food.pos.y) {
                valid_position = 0;
                break;
            }
        }
    } while (!valid_position);
    
    food.active = 1;
}

void draw_borders() {
    attron(COLOR_PAIR(COLOR_WALL));
    
    // Draw top and bottom borders
    for (int x = 0; x < GAME_WIDTH + 2; x++) {
        mvaddch(1, x + 1, ACS_CKBOARD);
        mvaddch(GAME_HEIGHT + 2, x + 1, ACS_CKBOARD);
    }
    
    // Draw left and right borders
    for (int y = 2; y < GAME_HEIGHT + 2; y++) {
        mvaddch(y, 1, ACS_CKBOARD);
        mvaddch(y, GAME_WIDTH + 2, ACS_CKBOARD);
    }
    
    // Draw corners
    mvaddch(1, 1, ACS_ULCORNER);
    mvaddch(1, GAME_WIDTH + 2, ACS_URCORNER);
    mvaddch(GAME_HEIGHT + 2, 1, ACS_LLCORNER);
    mvaddch(GAME_HEIGHT + 2, GAME_WIDTH + 2, ACS_LRCORNER);
    
    attroff(COLOR_PAIR(COLOR_WALL));
}

void draw_snake() {
    // Draw snake head
    attron(COLOR_PAIR(COLOR_SNAKE_HEAD) | A_BOLD);
    mvaddch(snake.body[0].y + 2, snake.body[0].x + 2, '@');
    attroff(COLOR_PAIR(COLOR_SNAKE_HEAD) | A_BOLD);
    
    // Draw snake body
    attron(COLOR_PAIR(COLOR_SNAKE_BODY));
    for (int i = 1; i < snake.length; i++) {
        mvaddch(snake.body[i].y + 2, snake.body[i].x + 2, 'O');
    }
    attroff(COLOR_PAIR(COLOR_SNAKE_BODY));
}

void draw_food() {
    if (food.active) {
        attron(COLOR_PAIR(COLOR_FOOD) | A_BOLD);
        mvaddch(food.pos.y + 2, food.pos.x + 2, '*');
        attroff(COLOR_PAIR(COLOR_FOOD) | A_BOLD);
    }
}

void draw_score() {
    attron(COLOR_PAIR(COLOR_SCORE) | A_BOLD);
    mvprintw(0, 2, "SNAKE GAME - Score: %d - Length: %d", score, snake.length);
    attroff(COLOR_PAIR(COLOR_SCORE) | A_BOLD);
    
    // Draw controls help
    mvprintw(GAME_HEIGHT + 4, 2, "Controls: Arrow Keys to move | P: Pause | Q: Quit | R: Restart");
}

void draw_game_over() {
    int center_x = (GAME_WIDTH - 20) / 2 + 2;
    int center_y = GAME_HEIGHT / 2 + 2;
    
    attron(COLOR_PAIR(COLOR_GAME_OVER) | A_BOLD);
    
    // Simple game over message
    mvprintw(center_y - 1, center_x, "GAME OVER");
    mvprintw(center_y, center_x, "Final Score: %d", score);
    mvprintw(center_y + 1, center_x, "Press R to restart");
    mvprintw(center_y + 2, center_x, "Press Q to quit");
    
    attroff(COLOR_PAIR(COLOR_GAME_OVER) | A_BOLD);
}

void draw_pause() {
    int center_x = (GAME_WIDTH - 16) / 2 + 2;
    int center_y = GAME_HEIGHT / 2 + 2;
    
    attron(COLOR_PAIR(COLOR_SCORE) | A_BOLD);
    
    // Simple pause message
    mvprintw(center_y, center_x, "GAME PAUSED");
    mvprintw(center_y + 1, center_x, "Press P to continue");
    
    attroff(COLOR_PAIR(COLOR_SCORE) | A_BOLD);
}

void move_snake() {
    // Move body segments
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    
    // Move head based on direction
    switch (snake.direction) {
        case 0: // Up
            snake.body[0].y--;
            break;
        case 1: // Right
            snake.body[0].x++;
            break;
        case 2: // Down
            snake.body[0].y++;
            break;
        case 3: // Left
            snake.body[0].x--;
            break;
    }
}

int check_collision() {
    Position head = snake.body[0];
    
    // Check wall collision
    if (head.x <= 0 || head.x >= GAME_WIDTH - 1 || 
        head.y <= 0 || head.y >= GAME_HEIGHT - 1) {
        return 1;
    }
    
    // Check self collision (skip first few segments to allow turning)
    for (int i = 4; i < snake.length; i++) {
        if (head.x == snake.body[i].x && head.y == snake.body[i].y) {
            return 1;
        }
    }
    
    return 0;
}

int check_food_collision() {
    if (food.active && 
        snake.body[0].x == food.pos.x && 
        snake.body[0].y == food.pos.y) {
        return 1;
    }
    return 0;
}

void grow_snake() {
    if (snake.length < snake.max_length) {
        snake.length++;
        // New segment will be added at the end in the next move
    }
}

void update_game() {
    if (game_over || game_paused) return;
    
    move_snake();
    
    if (check_collision()) {
        game_over = 1;
        return;
    }
    
    if (check_food_collision()) {
        score += 10;
        grow_snake();
        food.active = 0;
        generate_food();
    }
}

void handle_input() {
    int ch = getch();
    
    switch (ch) {
        case 'q':
        case 'Q':
            game_over = 2;  // Signal to quit
            break;
        case 'r':
        case 'R':
            if (game_over) {
                cleanup_game();
                init_game();
            }
            break;
        case 'p':
        case 'P':
            game_paused = !game_paused;
            break;
        case KEY_UP:
            if (snake.direction != 2) snake.direction = 0;
            break;
        case KEY_RIGHT:
            if (snake.direction != 3) snake.direction = 1;
            break;
        case KEY_DOWN:
            if (snake.direction != 0) snake.direction = 2;
            break;
        case KEY_LEFT:
            if (snake.direction != 1) snake.direction = 3;
            break;
    }
}

void draw_game() {
    clear();
    draw_score();
    draw_borders();
    draw_snake();
    draw_food();
    
    if (game_over == 1) {
        draw_game_over();
    } else if (game_paused) {
        draw_pause();
    }
    
    refresh();
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(0);
    
    if (!has_colors()) {
        endwin();
        printf("Your terminal does not support colors!\n");
        return 1;
    }
    
    init_colors();
    init_game();
    generate_food();
    
    // Main game loop
    while (1) {
        handle_input();
        
        if (game_over == 2) break;  // Quit signal
        
        if (!game_paused && !game_over) {
            update_game();
            if (!food.active) {
                generate_food();
            }
        }
        
        draw_game();
        
        if (!game_paused && !game_over) {
            usleep(GAME_SPEED);
        } else {
            usleep(100000);  // Shorter sleep when paused or game over
        }
    }
    
    cleanup_game();
    endwin();
    return 0;
}