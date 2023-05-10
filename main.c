#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "Deque.h"

#define ROWS 10
#define COLS 40
#define NFOODS 3
#define TIME_SLEEP_U (6 * 1000000 / 60) /* in microseconds (1/million seconds)*/

#define SCREEN_CLEAR() printf("\033[H\033[J")

enum Direction {
    UP, DOWN, LEFT, RIGHT, STOP
};

void initgrid(void);
void cleargame(void);
void printgrid(void);
void initsnakehead(void);
void addfood(void);
void addfoodn(int);
bool stepsnake(void);

static int grid[ROWS][COLS];
static Deque snakequeue;
static int numfreespots = ROWS*COLS;
static int snakesize = 0;
static enum Direction currdir = STOP;

int main(void)
{
    bool good;

    /* hide cursor */
    printf("\e[?25l");

    /* switch to canonical mode, disable echo */
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* initialize game */
    initgrid();
    deque_init(&snakequeue);
    initsnakehead();
    addfoodn(3);
    currdir = UP;

    /* game loop */
    good = true;
    while (good)
    {
        SCREEN_CLEAR();

        /* get user input */
        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        /* get user input */
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            int ch = getchar();
            if (ch == 'q')
                good = false;
            else if (ch == 'w' && currdir != DOWN)
                currdir = UP;
            else if (ch == 's' && currdir != UP)
                currdir = DOWN;
            else if (ch == 'd' && currdir != LEFT)
                currdir = RIGHT;
            else if (ch == 'a' && currdir != RIGHT)
                currdir = LEFT;
        }


        /* update */
        if (good)
            good = stepsnake();

        /* render */
        printgrid();

        if (!good)
            break;

        usleep(TIME_SLEEP_U);
    }

    printf("Game over!\n");

    /* show cursor */
    printf("\e[?25h");

    /* reset mode */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return 0;
}

void initgrid(void)
{
    int i, j;

    for (i = 0; i < ROWS; ++i)
        for (j = 0; j < COLS; ++j)
            grid[i][j] = 0;
}

void cleargame(void)
{
    initgrid();
    deque_clear(&snakequeue);
}

const char *getcellstr(int val)
{
    if (val > 0)
        return "▒"; // █
    if (val < 0)
        return "◉";
    return "·";
}

void printgrid(void)
{
    int i, j;

    printf("┌");
    for (j = 0; j < COLS; ++j)
        printf("─");
    printf("┐\n");

    for (i = 0; i < ROWS; ++i)
    {
        printf("│");
        for (j = 0; j < COLS; ++j)
            printf("%s", getcellstr(grid[i][j]));
        printf("│\n");
    }

    printf("└");
    for (j = 0; j < COLS; ++j)
        printf("─");
    printf("┘\n");
}

void initsnakehead(void)
{
    int x, y;

    y = rand() % ROWS;
    x = rand() % COLS;

    grid[y][x] = 1;

    ++snakesize;
    --numfreespots;

    deque_pushfront(&snakequeue, x, y);
}

void addfood()
{
    int i, j, count;

    if (numfreespots == 0)
        return;

    count = rand() % numfreespots;

    for (i = 0; i < ROWS; ++i)
        for (j = 0; j < COLS; ++j)
            if (grid[i][j] == 0 && count-- == 0)
            {
                --numfreespots;
                grid[i][j] = -1;
                return;
            }
}

void addfoodn(int n)
{
    while (n--)
        addfood();
}

bool stepsnake()
{
    Position tailpos, headpos;
    int dx, dy, newheadx, newheady, gridatnewhead;

    switch (currdir)
    {
    case UP:
        /* go up back substraction from rows */
        dx = 0;
        dy = -1;
        break;
    case DOWN:
        dx = 0;
        dy = 1;
        break;
    case LEFT:
        dx = -1;
        dy = 0;
        break;
    case RIGHT:
        dx = 1;
        dy = 0;
        break;
    case STOP:
        return false;
    default:
        fprintf(stderr, "stepsnake: unknown command\n");
        return false;
    }

    deque_front(&snakequeue, &headpos);
    deque_back(&snakequeue, &tailpos);

    newheadx = (headpos.x + dx + COLS) % COLS;
    newheady = (headpos.y + dy + ROWS) % ROWS;

    /* check if moving into food */
    if (grid[newheady][newheadx] < 0)
    {
        /* moved into food, don't remove tail */
        deque_pushfront(&snakequeue, newheadx, newheady);
        grid[newheady][newheadx] = 1;
        ++snakesize;
        /* replace food on grid */
        addfood();
        return true;
    }

    /* not moving into food; remove tail */
    deque_popback(&snakequeue);
    grid[tailpos.y][tailpos.x] = 0;

    /* check if empty spot after removing tail */
    if (grid[newheady][newheadx] == 0)
    {
        /* move into empty spot, move head */
        deque_pushfront(&snakequeue, newheadx, newheady);
        grid[newheady][newheadx] = 1;
        return true;
    }

    /* moved into self, game over */
    grid[tailpos.y][tailpos.x] = 1; /* don't remove last segment */
    return false;
}
