#include "widget.h" 

Button
*btn_init(int x, int y, int w, int h, SDL_Color bg, char *text, void (*on_click)()) {
    Button *btn = malloc(sizeof(Button));
    btn->x = x; btn->w = w;
    btn->y = y; btn->h = h;
    btn->color = bg;
    btn->text = text;
    btn->on_click = on_click;
    return btn;
}

int
btn_isover(Button *btn, int x, int y) {
    return (btn->x <= x && btn->y <= y && btn->x + btn->w >= x && btn->y + btn->h >= y);
}

void
btn_draw(SDL_Renderer *renderer, Button *btn, TTF_Font *font) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, btn->color.r, btn->color.g, btn->color.b, btn->color.a);
    SDL_Rect rect = { btn->x, btn->y, btn->w, btn->h };
    SDL_RenderFillRect(renderer, &rect);
    draw_text(renderer, btn->text, btn->x+btn->w/2, btn->y+btn->h/2, 1, font);
}

void
draw_text(SDL_Renderer *renderer, char *text, int x, int y, int center, TTF_Font *font) {
    SDL_Color color = {255,255,255};
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	int textW, textH;
	SDL_QueryTexture(texture, NULL,  NULL, &textW, &textH);
	SDL_Rect dstrect = { x, y, textW, textH };
    if (center) {
        dstrect.x -= textW / 2;
        dstrect.y -= textH / 2;
    }
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}

