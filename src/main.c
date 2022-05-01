#include <stdio.h>

#include "display.h"
#include "main.h"
#include "snake.h"
#include "food.h"
#include "celltile.h"
#include "widget.h"

/* SDL */
SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *t_snake;
SDL_Texture *tilemap;
TTF_Font *font = NULL;

/* Data */
snake *player = NULL;
food apple;

uint8_t running = 1;
uint8_t paused = 0;
uint64_t frame = 0;

uint8_t **board = NULL;

/* for fps capping */
uint64_t lasttime; 
uint64_t currtime;

/* btn */
Button *upbtn, *downbtn, *leftbtn, *rightbtn;

int
main(int argc, char *argv[]) {
    init();

    board = malloc(sizeof(uint8_t*) * g_height);
    if (board == NULL)
        die("Failed mallocing");

    for (int i = 0; i < g_height; i++)
        board[i] = calloc(g_width, sizeof(uint8_t));

    apple = generatefood(g_width, g_height, board);

    player = snake_init(g_width, g_height, board);

#ifndef __EMSCRIPTEN__
    while (running)
        gameloop();
#else
    emscripten_set_main_loop(gameloop, 0, 1);
#endif

    for (int i = 0; i < g_width; i++) {
        free(board[i]);
        board[i] = NULL;
    }
    free(board);

    SDL_DestroyTexture(tilemap);
    TTF_CloseFont(font);
    SDL_Quit();
    return 0;
}

void
die(const char *msg) {
    perror(msg);
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void
init(void) {
    uint64_t currtime = lasttime = SDL_GetTicks();

    /* init everything sdl */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) die(SDL_GetError());

    /* flags for initing window and renderer */
    uint32_t window_flags = SDL_WINDOW_SHOWN;
    uint32_t render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    /* init + check window and renderer */
    window = SDL_CreateWindow(TITLE, 0, 0, s_width, s_height, window_flags);

    /* -1 is index for capture card (default) */
    renderer = SDL_CreateRenderer(window, -1, render_flags); 
    if (!(window && renderer)) die(SDL_GetError());

    /* title */
    SDL_SetWindowTitle(window, TITLE);

    /* initing SDL_image */
#ifndef __EMSCRIPTEN__
    uint32_t img_flags = IMG_INIT_PNG;
    if (IMG_Init(img_flags) == 0) die(IMG_GetError()); 
#endif

    /* load image */
    SDL_Surface *tm_surface = IMG_Load("assets/tilemap.png");
    if (!tm_surface) die(IMG_GetError());

    tilemap = SDL_CreateTextureFromSurface(renderer, tm_surface);
    SDL_FreeSurface(tm_surface);

    /* SDL_TTF */
    if (TTF_Init() != 0) die(TTF_GetError());

    font = TTF_OpenFont(FONTPATH, fontsize);
    if (!font) die(TTF_GetError());

    SDL_Color btncolor = {100, 100, 100, 50};
    upbtn    = btn_init(70,  s_height - 130, 50, 50, btncolor, "", NULL);
    downbtn  = btn_init(70,  s_height - 70,  50, 50, btncolor, "", NULL);
    leftbtn  = btn_init(10,  s_height - 70,  50, 50, btncolor, "", NULL);
    rightbtn = btn_init(130, s_height - 70,  50, 50, btncolor, "", NULL);
}

void 
update(void) {
    if (frame % (FPS / MPS) == 0) {
        uint8_t state = snake_move(player);
        if (state == FOOD) {
            /* add score to player */
            player->score++;

            /* generate new apple ! */
            apple = generatefood(g_width, g_height, board);
        } else if (state == SNAKE) {
            /* self collision */
            paused = 1;
        }
    }
}

void 
input(void) {
    SDL_Event event;

    /* allow getting input as unicode string ([0] for ascii) */
    SDL_StartTextInput();
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
#ifdef __EMSCRIPTEN__ 
                emscripten_cancel_main_loop();
#endif
                break;
            case SDL_KEYDOWN:
                break;
            case SDL_TEXTINPUT:
                switch (event.text.text[0]) {
                    case 'w':
                    case 'a':
                    case 's':
                    case 'd':
                        snake_changedir(player, event.text.text[0]);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (btn_isover(upbtn, event.motion.x, event.motion.y))
                    snake_changedir(player, 'w');
                if (btn_isover(downbtn, event.motion.x, event.motion.y))
                    snake_changedir(player, 's');
                if (btn_isover(leftbtn, event.motion.x, event.motion.y))
                    snake_changedir(player, 'a');
                if (btn_isover(rightbtn, event.motion.x, event.motion.y))
                    snake_changedir(player, 'd');
                break;
            default:;
        }
    }
}

void 
render(void) {
    /* clear scr */
    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    SDL_RenderClear(renderer);

    drawbg();

    /* draw snake */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    node *iter = player->list;
    while (iter != NULL) {
        draw_snake_tile(iter);
        iter = iter->nex;
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    draw_snake_tile(player->head);

    /* draw apple */
    SDL_Rect dstrect = { apple.x * scale, apple.y * scale, scale, scale };
    SDL_RenderCopy(renderer, tilemap, &apple_srcrect, &dstrect);

    /* draw score text */
    char buf[50];
    snprintf(buf, 50, "SCORE: %d", player->score);
    drawtext(buf, s_width / 2, 100, 1, NULL);

    btn_draw(renderer, upbtn, font);
    btn_draw(renderer, leftbtn, font);
    btn_draw(renderer, downbtn, font);
    btn_draw(renderer, rightbtn, font);

    SDL_RenderPresent(renderer);
}

void
drawtext(char *text, int x, int y, int center, SDL_Color *color){ 
    SDL_Color black = { 0, 0, 0 };
    if (color == NULL) color = &black;
    SDL_Surface *temp = TTF_RenderText_Solid(font, text, *color);
    if (!temp)
        die(SDL_GetError());
    SDL_Texture *texttex = SDL_CreateTextureFromSurface(renderer, temp);
    if (!texttex)
        die(SDL_GetError());
    SDL_Rect dstrect = { x - (temp->w >> 1), y - (temp->h >> 1), temp->w, temp->h };
    SDL_RenderCopy(renderer, texttex, NULL, &dstrect);
    SDL_FreeSurface(temp);
    SDL_DestroyTexture(texttex);
}

void
drawbg(void) {
    SDL_Rect dstrect = { 0, 0, scale, scale };


    /* NULL mean pivot is center of the texture */
    SDL_Point *pivot = NULL;
    for (int i = 0; i < g_height; i++) {
        for (int j = 0; j < g_width; j++) {
            int angle = ((i+j) & 0x3) * 90;
            dstrect.x = j * scale;
            dstrect.y = i * scale;
            SDL_RenderCopyEx(renderer, tilemap, &bg_srcrect, &dstrect, angle, pivot, SDL_FLIP_NONE);
        }
    }
}

void
draw_snake_tile(node *cnode) {

    /* get last and next ptr */
    node *lnode = cnode->las;
    node *nnode = cnode->nex;

    const int dx[] = { -1, 0, 1,
                       -1,    1,
                       -1, 0, 1 };
    const int dy[] = { -1,-1,-1,
                        0,    0, 
                        1, 1, 1 };
    const SDL_Rect bg_srcrect = { 0, imagescale, imagescale, imagescale };

    uint8_t lnpos = 0x00;
    for (int i = 0; i < 8; i++) {
        if (lnode != NULL) {
            if (lnode->val.x == cnode->val.x + dx[i] && lnode->val.y == cnode->val.y + dy[i])
                lnpos |= (0x80 >> i);
        }
        if (nnode != NULL) {
            if (nnode->val.x == cnode->val.x + dx[i] && nnode->val.y == cnode->val.y + dy[i])
                lnpos |= (0x80 >> i);
        }
    }

    uint8_t body_type = STRAIGHT;
    if (cnode->nex == NULL) body_type = HEAD;
    else if (cnode->las == NULL) body_type = TAIL;

    uint16_t angle = 0;
    if (body_type == HEAD || body_type == TAIL) {
        for (int i = 0; i < 4; i++) {
            if (pattern_angle[body_type][i][0] == lnpos) {
                angle = pattern_angle[body_type][i][1];
            }
        }
    }
    else {
        for (int i = 0; i < 4; i++) {
            if (pattern_angle[TURNING][i][0] == lnpos) {
                angle = pattern_angle[TURNING][i][1];
                body_type = TURNING;
                break;
            }
            else if (pattern_angle[STRAIGHT][i][0] == lnpos) {
                angle = pattern_angle[STRAIGHT][i][1];
                body_type = STRAIGHT;
                break;
            }
        }
    }

    if (lnpos == 0x00) {
        angle = onlyhead_angle[player->dir];
    }

    SDL_Point *pivot = NULL;
    SDL_Rect dstrect = { cnode->val.x * scale, cnode->val.y * scale, scale, scale };
    SDL_RenderCopyEx( renderer, tilemap, snake_srcrect + body_type, &dstrect, angle, pivot, SDL_FLIP_NONE );
}

void
gameloop(void) {
    if (!paused)
        update();
    input();
    render();
    /* frame capping */
    currtime = SDL_GetTicks();
    uint64_t delta = currtime - lasttime;
    if (delta < timestep)
        SDL_Delay(timestep - currtime + lasttime);
    else
        lasttime = currtime;
    frame++;
}

