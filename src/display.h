#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>

/*extern int s_width;
extern int s_height;
extern uint8_t g_width;
extern uint8_t g_height;*/
static const uint8_t scale = 32;
static const uint8_t g_width = 36;
static const uint8_t g_height = 24;
static const uint16_t s_width = g_width * scale;
static const uint16_t s_height = g_height * scale;
static const uint8_t imagescale = 16;
static const uint8_t fontsize = 64;
    
#endif
