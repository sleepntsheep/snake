#include <stdlib.h>
#include <time.h>
#include "celltile.h"
#include "food.h"

food
generatefood(uint16_t board_width, uint16_t board_height, uint8_t **board) {
    srand(time(NULL));
    food new = { rand() % board_width, rand() % board_height };
    while (board[new.y][new.x] != EMPTY) {
        new.x = rand() % board_width;
        if (board[new.y][new.x] == EMPTY) break;
        new.y = rand() % board_height;
    }
    board[new.y][new.x] = FOOD;
    return new;
}
