CC := gcc
MW := winegcc
CCFLAGS := -std=c11 -Wall
LFLAGS := -lSDL2 -lSDL2_ttf -lSDL2_image 
SRCS := src/*.c

.PHONY: all build run wasm web

build:
	$(CC) $(SRCS) -o snake $(CCFLAGS) $(LFLAGS)

run:
	make build
	./snake

wn:
	$(MW) $(SRCS) -o snake -e main $(CCFLAGS) $(LFLAGS)

wasm:
	emcc $(SRCS) -o snake $(CCFLAGS) $(LFLAGS) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s STANDALONE_WASM  -s WASM_ASYNC_COMPILATION=0 --preload-file assets --use-preload-plugins -s SDL2_IMAGE_FORMATS="[\"png\"]"

web:
	emcc $(SRCS) -o web/snake.html $(CCFLAGS) $(LFLAGS) -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 --preload-file assets -s EXIT_RUNTIME=1 --use-preload-plugins -s SDL2_IMAGE_FORMATS="[\"png\"]" -s GL_UNSAFE_OPTS=0 -s STB_IMAGE=1


