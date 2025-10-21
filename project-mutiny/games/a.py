import curses
import random
import time

# Game settings
WIDTH = 40
HEIGHT = 20
PLAYER = "A"  # Assassin
ENEMY = "E"
ITEM = "I"
EMPTY = " "
WALL = "#"
EXIT = "X"

# Initialize the screen
stdscr = curses.initscr()
curses.curs_set(0)
stdscr.timeout(100)
sh, sw = stdscr.getmaxyx()
win = curses.newwin(HEIGHT, WIDTH, (sh // 2) - (HEIGHT // 2), (sw // 2) - (WIDTH // 2))
win.keypad(1)

# Game state
player_pos = [1, 1]
exit_pos = [HEIGHT - 2, WIDTH - 2]
enemies = [
    [random.randint(1, HEIGHT - 2), random.randint(1, WIDTH - 2)] for _ in range(3)
]
items = [
    [random.randint(1, HEIGHT - 2), random.randint(1, WIDTH - 2)] for _ in range(3)
]
score = 0


# Draw the game board
def draw_board():
    win.clear()

    for y in range(HEIGHT):
        for x in range(WIDTH):
            try:
                if [y, x] == player_pos:
                    win.addch(y, x, PLAYER)
                elif [y, x] == exit_pos:
                    win.addch(y, x, EXIT)
                elif [y, x] in enemies:
                    win.addch(y, x, ENEMY)
                elif [y, x] in items:
                    win.addch(y, x, ITEM)
                elif x == 0 or x == WIDTH - 1 or y == 0 or y == HEIGHT - 1:
                    win.addch(y, x, WALL)
                else:
                    win.addch(y, x, EMPTY)
            except curses.error:
                pass
    win.refresh()


# Main game loop
while True:
    draw_board()
    key = win.getch()

    if key == ord("q"):
        break

    # Move player
    if key == curses.KEY_UP and player_pos[0] > 1:
        player_pos[0] -= 1
    elif key == curses.KEY_DOWN and player_pos[0] < HEIGHT - 2:
        player_pos[0] += 1
    elif key == curses.KEY_LEFT and player_pos[1] > 1:
        player_pos[1] -= 1
    elif key == curses.KEY_RIGHT and player_pos[1] < WIDTH - 2:
        player_pos[1] += 1

    # Check for item collection
    if player_pos in items:
        items.remove(player_pos)
        score += 10

    # Check for enemy collision
    if player_pos in enemies:
        win.addstr(HEIGHT // 2, WIDTH // 2 - 5, "Game Over!")
        win.refresh()
        time.sleep(2)
        break

    # Check for exit
    if player_pos == exit_pos:
        win.addstr(HEIGHT // 2, WIDTH // 2 - 5, "You Win!")
        win.refresh()
        time.sleep(2)
        break

# End the game
curses.endwin()
