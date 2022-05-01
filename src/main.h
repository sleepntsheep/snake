#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "snake.h"

#define TITLE "Snake Game"

#define ASSETSPATH "assets/"
#define FONTPATH ASSETSPATH"sheepfn.ttf"

static const int FPS = 60;
static const int timestep = 1000 / FPS;
static const int MPS = 6; /* move per sec : mustn't be more than fps */

/* srcrect for snake.tga sprite sheet */
static const SDL_Rect snake_srcrect[] = {
    { 0, 0, imagescale, imagescale },
    { imagescale, 0, imagescale, imagescale },
    { imagescale << 1, 0, imagescale, imagescale },
    { imagescale * 3, 0, imagescale, imagescale },
};

static const SDL_Rect bg_srcrect = { 0, imagescale, imagescale, imagescale };
static const SDL_Rect apple_srcrect = { imagescale, imagescale, imagescale, imagescale };

void
die(const char *msg);

void
draw_snake_tile(node *node);

void 
update(void);

void 
input(void);

void
drawbg(void);

void 
render(void);

void
drawtext(char *text, int x, int y, int center, SDL_Color *color);

void
gameloop();

void
init(void);
#endif
