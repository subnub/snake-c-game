#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <time.h>

struct snake
{
    int x;
    int y;
    int prevX;
    int prevY;
};

struct field
{
    int width;
    int height;
};

struct food
{
    int x;
    int y;
};

struct snake snake = {.x = 10, .y = 10};

struct field field = {.width = 60, .height = 30};

struct food foodList[5];

struct snake *snakeList;

char **fieldMap;

int GAMEOVER = 0;
int SCORE = 0;
char LASTPRESSEDKEY = 'd';

double highestDrawTime = 0.0;

void sleepSecMillis(int seconds, int milliseconds)
{
    struct timespec ts = {seconds, milliseconds * 1000000L};
    nanosleep(&ts, NULL);
}

void checkInput()
{
    int pressedKey;

    pressedKey = getch();

    pressedKey = pressedKey == -1 ? LASTPRESSEDKEY : pressedKey;

    struct snake *snakeHead = snakeList;

    snakeHead->prevX = snakeHead->x;
    snakeHead->prevY = snakeHead->y;

    fieldMap[snakeHead->prevX][snakeHead->prevY] = ' ';

    switch (pressedKey)
    {
    case 'c':
    {
        GAMEOVER = 1;
        break;
    }
    case 'w':
    {
        snakeHead->y--;
        LASTPRESSEDKEY = 'w';
        break;
    }
    case 's':
    {
        snakeHead->y++;
        LASTPRESSEDKEY = 's';
        break;
    }
    case 'd':
    {
        snakeHead->x++;
        LASTPRESSEDKEY = 'd';
        break;
    }
    case 'a':
    {
        snakeHead->x--;
        LASTPRESSEDKEY = 'a';
        break;
    }
    }

    if (snakeHead->x >= field.width || snakeHead->y >= field.height || snakeHead->x < 0 || snakeHead->y < 0)
    {
        return;
    }

    fieldMap[snakeHead->x][snakeHead->y] = 'O';

    for (int i = 1; i <= SCORE; i++)
    {
        struct snake *pSnake = snakeList + i;
        struct snake *pPreviousSnake = snakeList + (i - 1);
        pSnake->prevX = pSnake->x;
        pSnake->prevY = pSnake->y;
        pSnake->x = pPreviousSnake->prevX;
        pSnake->y = pPreviousSnake->prevY;
        if (pSnake->prevX != 0 && pSnake->prevY != 0)
        {
            fieldMap[pSnake->prevX][pSnake->prevY] = ' ';
        }
        fieldMap[pSnake->x][pSnake->y] = 'O';
    }
}

void randomizeFood(struct food *pFood)
{
    pFood->x = (rand() % (field.width - 2)) + 1;
    pFood->y = (rand() % (field.height - 2)) + 1;
}

void initialFieldMapFill()
{
    for (int x = 0; x < field.width; x++)
    {
        for (int y = 0; y < field.height; y++)
        {
            if (y == 0 || y == field.height - 1)
            {
                fieldMap[x][y] = '#';
            }
            else if (x == 0 || x == field.width - 1)
            {
                fieldMap[x][y] = '#';
            }
            else
            {
                fieldMap[x][y] = ' ';
            }
        }
    }
}

void setup()
{
    GAMEOVER = 0;

    snakeList = calloc(1, sizeof(snake));
    struct snake snakeHead = {.x = 10, .y = 10, .prevX = 9, .prevY = 10};
    snakeList[0] = snakeHead;

    fieldMap = (char **)calloc(field.width, sizeof(char *));
    for (int i = 0; i < field.width; i++)
    {
        fieldMap[i] = calloc(field.height, sizeof(char));
    }

    initialFieldMapFill();

    for (int i = 0; i < 5; i++)
    {
        struct food *pFood = foodList + i;
        randomizeFood(pFood);
        fieldMap[pFood->x][pFood->y] = '*';
    }
}

void draw()
{
    for (int x = 0; x < field.height; x++)
    {
        for (int y = 0; y < field.width; y++)
        {
            if (fieldMap[y][x] == '#')
            {
                attron(COLOR_PAIR(1));
                printw("%c", fieldMap[y][x]);
                attroff(COLOR_PAIR(1));
            }
            else if (fieldMap[y][x] == 'O')
            {
                attron(COLOR_PAIR(2));
                printw("%c", fieldMap[y][x]);
                attroff(COLOR_PAIR(2));
            }
            else if (fieldMap[y][x] == '*')
            {
                attron(COLOR_PAIR(3));
                printw("%c", fieldMap[y][x]);
                attroff(COLOR_PAIR(3));
            }
            else
            {
                printw("%c", fieldMap[y][x]);
            }
        }
        printw("\n");
    }
    printw("Score: %d\n", SCORE);
}

void logic()
{

    struct snake *snakeHead = snakeList;

    if (snakeHead->x == 0 || snakeHead->x == field.width - 1 || snakeHead->y == 0 || snakeHead->y == field.height - 1)
    {
        GAMEOVER = 1;
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        struct food *pFood = foodList + i;
        if (pFood->x == snakeHead->x && pFood->y == snakeHead->y)
        {
            SCORE += 1;
            snakeList = realloc(snakeList, sizeof(snake) * (SCORE + 1));
            snakeList[SCORE].prevX = 0;
            snakeList[SCORE].prevY = 0;
            snakeList[SCORE].x = 0;
            snakeList[SCORE].y = 0;
            fieldMap[pFood->x][pFood->y] = ' ';
            randomizeFood(pFood);
            fieldMap[pFood->x][pFood->y] = '*';
            break;
        }
    }

    for (int i = 1; i <= SCORE; i++)
    {
        struct snake *currentSnake = snakeList + i;
        if (currentSnake->x == snakeHead->x && currentSnake->y == snakeHead->y)
        {
            GAMEOVER = 1;
            break;
        }
    }
}

int main(int argc, char const *argv[])
{
    initscr();
    cbreak();
    noecho();
    raw();
    nodelay(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    setup();

    while (!GAMEOVER)
    {
        clear();
        logic();
        checkInput();

        clock_t start_time, end_time;
        start_time = clock();
        draw();
        end_time = clock();

        double total_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
        if (total_time > highestDrawTime)
        {
            highestDrawTime = total_time;
        }

        refresh();
        int sleepTime = (LASTPRESSEDKEY == 'w' || LASTPRESSEDKEY == 's') ? 90 : 45;
        sleepSecMillis(0, sleepTime);
    }

    endwin();

    free(snakeList);

    for (int i = 0; i < field.height; i++)
    {
        free(fieldMap[i]);
    }
    free(fieldMap);

    printf("Game Over! Score: %d Highest render time: %f\n", SCORE, highestDrawTime);

    return 0;
}