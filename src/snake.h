#ifndef SNAKE_H
#define SNAKE_H
#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"

enum Direction {
    STATIC,
    UP,
    LEFT,
    DOWN,
    RIGHT
};

enum SnakeTileMap {
    STRAIGHT,
    TURNING,
    HEAD,
    TAIL
};

static const uint16_t pattern_angle[][4][2] = { 
    [STRAIGHT] = {
        {0x42, 0}, {0x18, 90}, {0xFF, 0}, {0xFF, 0}
    },
    [TURNING] = {
        {0x0A, 0}, {0x12, 90}, {0x50, 180}, {0x48, 270},
    },
    [HEAD] = {
        {0x10, 0}, {0x08, 180}, {0x40, 90}, {0x02, 270},
    },
    [TAIL] = {
        {0x10, 180}, {0x08, 0}, {0x40, 270}, {0x02, 90},
    },
};

static const uint16_t onlyhead_angle[5] = { 0, 270, 180, 90, 0 };

typedef struct pos pos;
typedef struct node node;

struct pos {
    int16_t x;
    int16_t y;
};

struct node {
    pos val;
    node *nex;
    node *las;
};

typedef struct {
    node* list;
    node* head;
    uint16_t length;
    uint8_t dir;
    uint16_t boardwidth;
    uint16_t boardheight;
    uint32_t score;
    uint8_t **board;
} snake;

snake *
snake_init(uint16_t boardwidth, uint16_t boardheight, uint8_t **board);

void
snake_add(snake *sn, pos nwpos);

void
snake_remove(snake *sn);

uint8_t 
snake_move(snake *sn);

void
snake_check_bound(snake *sn, pos *ps);

void
snake_changedir(snake *sn, char c);
#endif
