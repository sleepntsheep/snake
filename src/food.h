#ifndef FOOD_H
#define FOOD_H
#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} food;

food
generatefood(uint16_t board_width, uint16_t board_height, uint8_t **board);

void
foodsrand();

#endif
