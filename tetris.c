#include "tetris.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

char grid[HEIGHT][WIDTH];
tetramino_t *current_tetramino = NULL;
bool game_over = false;

static struct termios orig_term;

void init_grid() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = '.';
        }
    }
}

void print_grid() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

void init_terminal(struct termios *term) {
    printf("\x1b[?25l"); /* Hide cursor */

    /* Save current terminal settings */
    tcgetattr(STDIN_FILENO, &orig_term);
    *term = orig_term;

    /* Disable echo and canonical mode */
    term->c_lflag &= ~ECHO;
    term->c_lflag &= ~ICANON;

    /* Make it polling read (non-blocking) */
    term->c_cc[VMIN] = 0;
    term->c_cc[VTIME] = 0;
    /* Apply changes */
    tcsetattr(STDIN_FILENO, 0, term);
}

void deinit_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
    printf("\x1b[?25h"); /* Make cursor visible */
}

void clock_gettime_helper(struct timespec *t) {
    if (clock_gettime(CLOCK_REALTIME, t) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
}

void get_new_tetramino() {
    /* TODO: implement randomly selecting from all available tetraminos */
    /* For time being, create L piece */
    current_tetramino->center = (point_t){.x = 4, .y = 1};
    current_tetramino->tetraminos[0] = (point_t){.x = -1, .y = 0};
    current_tetramino->tetraminos[1] = (point_t){.x = 0, .y = 0};
    current_tetramino->tetraminos[2] = (point_t){.x = 1, .y = 0};
    current_tetramino->tetraminos[3] = (point_t){.x = 1, .y = -1};

    current_tetramino->active = true;

    /* Can the piece be legally placed? If not, the game is lost */
    bool legal = is_legal_move(0, 0);
    if (!legal)
        game_over = true;

    /* Discard input from previous tetramino */
    tcflush(STDIN_FILENO, TCIFLUSH);
}

void place_tetramino_grid() {
    for (int i = 0; i < 4; i++) {
        int x =
            current_tetramino->center.x + current_tetramino->tetraminos[i].x;
        int y =
            current_tetramino->center.y + current_tetramino->tetraminos[i].y;

        grid[y][x] = '#';
    }

    if (current_tetramino->active == false)
        check_for_full_lines();
}

void unplace_tetramino_grid() {
    for (int i = 0; i < 4; i++) {
        int x =
            current_tetramino->center.x + current_tetramino->tetraminos[i].x;
        int y =
            current_tetramino->center.y + current_tetramino->tetraminos[i].y;

        grid[y][x] = '.';
    }
}

bool is_legal_move(int dx, int dy) {
    for (int i = 0; i < 4; i++) {
        int x = current_tetramino->center.x +
                current_tetramino->tetraminos[i].x + dx;
        int y = current_tetramino->center.y +
                current_tetramino->tetraminos[i].y + dy;

        /* If the x-coordinate is invalid OR
         *    the y-coordinate is invalid OR
         * the space is occupied */
        if ((x < 0 || x >= WIDTH) || (y < 0 || y >= HEIGHT) ||
            (grid[y][x] == '#')) {
            /* Illegal move */
            return false;
        }
    }

    return true;
}

void rotate_piece_helper(bool clockwise) {
    int newx;
    int newy;
    for (int i = 0; i < 4; i++) {
        if (clockwise) {
            newx = -current_tetramino->tetraminos[i].y;
            newy = current_tetramino->tetraminos[i].x;
        } else {
            newx = current_tetramino->tetraminos[i].y;
            newy = -current_tetramino->tetraminos[i].x;
        }
        current_tetramino->tetraminos[i].x = newx;
        current_tetramino->tetraminos[i].y = newy;
    }
}

void rotate_piece() {
    rotate_piece_helper(true);

    bool legal = is_legal_move(0, 0);
    if (!legal) {
        /* TODO: implement rotation kick */
        /* We could not rotate, rotate back */
        rotate_piece_helper(false);
    }
}

void check_for_full_lines() {
    int rows_to_be_removed[4] = {-1, -1, -1, -1};
    int num_full_lines = 0;

    for (int row = HEIGHT - 1; row >= 0; row--) {
        int col = 0;
        while (grid[row][col++] == '#') {
            if (col == WIDTH) {
                /* This row is full and should be scheduled for removal */
                rows_to_be_removed[num_full_lines++] = row;
                break;
            }
        }
    }

    /* A maximum of four lines can be full at once */
    for (int i = 0; i < 4; i++) {
        if (rows_to_be_removed[i] == -1)
            break;

        /* Add i since i rows have already been moved down */
        int row = rows_to_be_removed[i] + i;

        /* For every row above (and including) this row */
        while (row > num_full_lines) {
            /* Replace each row with the one above */
            for (int col = 0; col < WIDTH; col++) {
                /* TODO: */
                /* row - 1 should not cause a bug for the moment because of
                 * where new pieces are created on the grid but might cause a
                 * problem in the future */
                grid[row][col] = grid[row - 1][col];
            }

            row--;
        }
    }

    /* Fill top rows with nothing */
    for (int row = 0; row < num_full_lines; row++) {
        for (int col = 0; col < WIDTH; col++) {
            grid[row][col] = '.';
        }
    }
}

void move_piece(char c) {
    bool legal;
    switch (c) {
    case 'a':
        legal = is_legal_move(-1, 0);
        if (legal)
            current_tetramino->center.x--;
        break;
    case 'd':
        legal = is_legal_move(1, 0);
        if (legal)
            current_tetramino->center.x++;
        break;
    case 's':
        legal = is_legal_move(0, 1);
        if (legal)
            current_tetramino->center.y++;
        else {
            /* Moving the piece down is illegal, which means that the piece
             * should be placed here */
            current_tetramino->active = false;
        }
        break;
    case 'r':
    case 'w':
        rotate_piece();
        break;
    }
}

void update(char c) {
    static int ticks = 0;

    static struct timespec prev_time;
    static bool timespec_initialised = false;
    /* TODO: implement key repeat logic */

    if (!timespec_initialised) {
        clock_gettime_helper(&prev_time);
        timespec_initialised = true;
    }

    if (current_tetramino->active == false) {
        /* A new tetramino should be created at the top of the screen */
        get_new_tetramino();
    } else {
        unplace_tetramino_grid();

        move_piece(c);
    }

    /* Gravity at 1 FPS */
    if ((ticks % FPS) == 0) {
        /* Move piece down */
        bool legal = is_legal_move(0, 1);
        if (legal)
            current_tetramino->center.y++;
        else {
            /* Moving the piece down is illegal, which means that the piece
             * should be placed here */
            current_tetramino->active = false;
        }
    }
    place_tetramino_grid();
    ticks++;
}

int get_key(void) {
    unsigned char c = 0;
    ssize_t bytes_read = read(STDIN_FILENO, &c, 1);

    if (bytes_read == 1) {
        return (int)c;
    }
    return -1;
}

/* Version that clears the input buffer */
/* int get_key(void) { */
/*     unsigned char c; */
/*     int latest_key = -1; */

/*     /\* Clear the input buffer *\/ */
/*     while (read(STDIN_FILENO, &c, 1) == 1) { */
/*         latest_key = (int)c; */
/*     } */

/*     return latest_key; */
/* } */

int main(int argc, char *argv[]) {
    init_grid();

    struct termios term;
    init_terminal(&term);

    const double frame_time_microseconds = (1.0 / FPS) * 1e6;

    current_tetramino = malloc(sizeof(tetramino_t));
    current_tetramino->active = false;

    int key;
    char c;
    int i = 0;
    struct timespec start, end;
    while (!game_over) {
        key = get_key();
        if (key == -1) {
            /* No input */
            c = ' ';
        } else {
            c = (char)key;
        }

        clock_gettime_helper(&start);

        update(c);

        print_grid();

        printf("\x1b[%dA", HEIGHT); /* move cursor up HEIGHT lines */
        printf("\x1b[0G");          /* move cursor to column 0 */
        i++;

        clock_gettime_helper(&end);

        double elapsed_microseconds = (end.tv_sec - start.tv_sec) * 1e6 +
                                      (end.tv_nsec - start.tv_nsec) / 1e3;

        /* TODO: what happens if elapsed time is longer than frametime?
         */
        usleep(frame_time_microseconds - elapsed_microseconds);
    }

    print_grid();
    printf("Game Over!");

    deinit_terminal();
    free(current_tetramino);
    return EXIT_SUCCESS;
}

// clang-format off
// Local Variables:
// compile-command: "gcc -o tetris tetris.c"
// End:
// clang-format on
