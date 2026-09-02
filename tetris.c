#include "getch.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define WIDTH (10)
#define HEIGHT (20)
#define FPS (2)

void print_grid(int num) {
        for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                        printf("%d", num % 10);
                }
                printf("\n");
        }
}

int main(int argc, char *argv[]) {
        printf("\x1b[?25l"); /* Hide cursor */

        /* Disable echo in terminal */
        struct termios term;
        tcgetattr(fileno(stdin), &term);

        term.c_lflag &= ~ECHO;
        tcsetattr(fileno(stdin), 0, &term);

        const double frame_time_microseconds = (1.0 / FPS) * 1e6;

        char c;
        int i = 0;
        struct timespec start, end;
        while ((c = getch()) != 'c') {
                if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
                        perror("clock_gettime");
                        exit(EXIT_FAILURE);
                }
                print_grid(i);
                printf("\x1b[%dA", HEIGHT); /* move cursor up HEIGHT lines */
                printf("\x1b[0G");          /* move cursor to column 0 */
                i++;

                if (clock_gettime(CLOCK_REALTIME, &end) == -1) {
                        perror("clock_gettime");
                        exit(EXIT_FAILURE);
                }

                double elapsed_microseconds =
                    (end.tv_sec - start.tv_sec) * 1e6 +
                    (end.tv_nsec - start.tv_nsec) / 1e3;

                /* TODO: what happens if elapsed time is longer than frametime?
                 */
                usleep(frame_time_microseconds - elapsed_microseconds);
        }

        /* Enable echo in the terminal */
        term.c_lflag |= ECHO;
        tcsetattr(fileno(stdin), 0, &term);
        printf("\x1b[?25h"); /* Make cursor visible */
        return EXIT_SUCCESS;
}

// clang-format off
// Local Variables:
// compile-command: "gcc -o tetris tetris.c getch.c"
// End:
// clang-format on
