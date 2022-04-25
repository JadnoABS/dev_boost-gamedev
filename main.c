#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <math.h>

#define SMALL 0.00001

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef enum { Top, Right, Bottom, Left } Direction;

typedef struct {
  SDL_Renderer *renderer;
  SDL_Window *window;
  bool running;
} Game;

typedef struct {
  SDL_Rect body;
  bool running;
  Direction currentD;
  Direction nextD;
} Player;

void drawPlayer(Game *game, Player *player);

int main() {

  Game *game = malloc(sizeof(Game));
  Player *player = malloc(sizeof(Player));

  game->running = true;

  player->body.x = 10;
  player->body.y = 10;
  player->body.w = 10;
  player->body.h = 10;

  player->running = false;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error on initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  atexit(SDL_Quit);

  game->window = SDL_CreateWindow("Janela teste", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!game->window) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    return 1;
  }

  game->renderer =
      SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

  // Game Loop
  while (game->running) {
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderClear(game->renderer);

    // Event Handling
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

      if (e.type == SDL_QUIT ||
          (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
        game->running = false;

      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w:
          player->running = true;
          player->currentD = Top;
          break;
        case SDLK_d:
          player->running = true;
          player->currentD = Right;

          break;
        case SDLK_s:
          player->running = true;
          player->currentD = Bottom;
          break;
        case SDLK_a:
          player->running = true;
          player->currentD = Left;
          break;
        }
      }
    }

    printf("%d\n", player->currentD);

    if (player->running) {

      switch (player->currentD) {
      case Top:
        player->body.y -= 2;
        break;
      case Right:
        player->body.x += 2;
        break;
      case Bottom:
        player->body.y += 2;
        break;
      case Left:
        player->body.x -= 2;
        break;
      }
    }

    drawPlayer(game, player);

    // Screen update
    SDL_RenderPresent(game->renderer);
    SDL_Delay(1000 / 60);
  }

  SDL_DestroyWindow(game->window);

  SDL_Quit();

  return 0;
}

void drawPlayer(Game *game, Player *player) {

  SDL_SetRenderDrawColor(game->renderer, 255, 100, 100, 255);

  SDL_RenderFillRect(game->renderer, &player->body);
}
