#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <termios.h>
#include <time.h>

#define WIDTH (10)
#define HEIGHT (20)
#define FPS (2)

/* Data structures */

typedef struct point_t {
    int x;
    int y;
} point_t;

typedef struct tetramino_t {
    point_t center;
    point_t tetraminos[4];
    bool active;
} tetramino_t;

/* Global state declarations */

extern char grid[HEIGHT][WIDTH];
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
