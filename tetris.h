#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <termios.h>
#include <time.h>

#define WIDTH (10)
#define HEIGHT (20)
#define FPS (60)

/* Data structures */
typedef enum {
    BLOCK_EMPTY = 0,
    BLOCK_CYAN,
    BLOCK_BLUE,
    BLOCK_ORANGE,
    BLOCK_YELLOW,
    BLOCK_GREEN,
    BLOCK_PURPLE,
    BLOCK_RED,
    NUM_BLOCK_TYPES
} BlockType;

const char *colours[NUM_BLOCK_TYPES] = {
    "",         "\x1b[36m", "\x1b[34m", "\x1b[38;5;208m",
    "\x1b[33m", "\x1b[32m", "\x1b[35m", "\x1b[31m"};

#define COLOUR_RESET "\x1b[0m"
typedef struct point_t {
    int x;
    int y;
} point_t;

typedef struct tetramino_t {
    point_t center;
    point_t tetraminos[4];
    bool active;
    BlockType blocktype;
} tetramino_t;

/* Global state declarations */
extern BlockType grid[HEIGHT][WIDTH];
extern tetramino_t *current_tetramino;

/* Board and game logic functions */

void init_grid(void);
void print_grid(void);
void get_new_tetramino(void);
void place_tetramino_grid(void);
void unplace_tetramino_grid(void);
bool is_legal_move(int dx, int dy);
void move_piece(char c);
void rotate_piece(void);
void check_for_full_lines();
void update(char c);
int get_key(void);

/* Terminal & timing utilities */

void init_terminal(struct termios *term);
void deinit_terminal();
void clock_gettime_helper(struct timespec *t);

#endif /* TETRIS_H */
