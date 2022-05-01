#include <stdlib.h>
#include "snake.h"
#include "celltile.h"

#define ABS(a) (((a) > 0) ? (a) : -(a))

snake *
snake_init(uint16_t boardwidth, uint16_t boardheight, uint8_t **board) {
    snake *sn = (snake *) malloc(sizeof(snake));
    sn->list = (node *) malloc(sizeof(node));
    pos temp = {boardwidth / 2, boardheight / 2};
    sn->list->val = temp;
    sn->list->nex = NULL;
    sn->list->las = NULL;
    sn->dir = UP;
    sn->head = sn->list;
    sn->boardwidth  = boardwidth;
    sn->boardheight = boardheight;
    sn->board = board;
    sn->score = 0;
    return sn;
}

void
snake_add(snake *sn, pos nwpos) {
    node *new = (node *) malloc(sizeof(node));
    new->val = nwpos;
    new->nex = NULL;
    new->las = sn->head;
    sn->head->nex = new;
    sn->head = new;

    sn->length++;
}

void
snake_remove(snake *sn) {
    node * toremove = sn->list;
    sn->list = sn->list->nex;
    sn->list->las = NULL;
    free(toremove);
    sn->length--;
}

uint8_t
snake_move(snake *sn) {
    /* return state of cell that head is in before changed */
    node *hd = sn->head;

    pos oldtail = sn->list->val;

    /* distance to move for x and y axis of each direction */
    const int8_t dx[] = { 0, 0, -1, 0, 1 };
    const int8_t dy[] = { 0, -1, 0, 1, 0 };

    pos newp = hd->val;

    /* calc new position of head */
    newp.x += dx[sn->dir];
    newp.y += dy[sn->dir];
    snake_check_bound(sn, &newp);

    /* get old state of new pos */
    uint8_t state = sn->board[newp.y][newp.x];

    if (oldtail.x == newp.x && oldtail.y == newp.y)
        state = EMPTY;

    /* update snake */
    snake_add(sn, newp);

    /* update cell in board */
    sn->board[newp.y][newp.x] = SNAKE;

    /* if snake ate food, don't cut its tail */
    if (state != FOOD) {
        sn->board[oldtail.y][oldtail.x] = EMPTY;
        snake_remove(sn);
    }

    return state;
}

void
snake_check_bound(snake *sn, pos *ps) {
    if (ps->x < 0)
        ps->x = sn->boardwidth - 1;
    else if (ps->x >= sn->boardwidth)
        ps->x = 0;
    if (ps->y < 0)
        ps->y = sn->boardheight - 1;
    else if (ps->y >= sn->boardheight)
        ps->y = 0;
}

void
snake_changedir(snake *sn, char c) {
    uint8_t olddir = sn->dir;
    switch (c) {
        case 'w':
            sn->dir = UP;
            break;
        case 'a':
            sn->dir = LEFT;
            break;
        case 's':
            sn->dir = DOWN;
            break;
        case 'd':
            sn->dir = RIGHT;
            break;
        default:;
    }
    if (ABS(sn->dir - olddir) == 2 && sn->length > 0) /* opposite */
        sn->dir = olddir;
}
