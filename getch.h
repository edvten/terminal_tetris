#ifndef GETCH_H
#define GETCH_H

/* Taken from: https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux */

/* Initialize new terminal i/o settings */
void initTermios(int echo);

/* Restore old terminal i/o settings */
void resetTermios(void);

/* Read 1 character - echo defines echo mode */
char getch_(int echo);

/* Read 1 character without echo */
char getch(void);

/* Read 1 character with echo */
char getche(void);

#endif                          /* GETCH_H */
