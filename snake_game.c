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

time_t t;

const int PLAYER_INI_POS_X = 80;
const int PLAYER_INI_POS_Y = 320;
const int PLAYER_SIZE_LIMB = 40;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

typedef enum { Top, Right, Bottom, Left } Direction;

typedef struct {
  SDL_Renderer *renderer;
  SDL_Window *window;
  bool running;
} Game;

typedef struct {
  int x;
  int y;
} Coordinate;

/* estrutura que implementa um pedaço do corpo da cobra
 */
typedef struct aux {
  SDL_Rect rect;
  Direction currentDir;     // direção atual daquele pedaço de corpo
  Coordinate lastValidCoor; // útlima coordenada valida daquele pedaço de corpo
  struct aux *prox;
  struct aux *ant;
} Limb;

/* size corresponde ao tamanho somente do corpo, isto é, não inclui a cabeça.
 * Body foi declarado como um vetor dinâmico. body[0] corresponde a cabeça e os
 * demais índices correspondem ao restante do corpo
 */
typedef struct {
  Limb *body;
  bool running;
  Direction nextDir;
  int size;
} Player;

typedef struct {
  SDL_Rect rect;
} Maca;

// funcao para atribuir valor a algumas variaveis
void inicializar(Game *game, Player *player) {
  srand((unsigned)time(&t));
  game->running = true;
  // definindo o tamanho inicial da cobra;
  player->size = 1;

  // a ideia de que seja alocado um espa�o a mais � que um desses espa�os
  // corresponderia a cabe�a,
  player->body = (Limb *)malloc(sizeof(Limb));
  player->body->prox = NULL;
  player->body->ant = NULL;
  //
  player->nextDir = Right;
  player->body->currentDir = Right;
  // player->body[1].currentDir = Right;
  // player->body[2].currentDir = Right;

  // settamos os valores iniciais da cabe�a
  player->body->rect.x = PLAYER_INI_POS_X;
  player->body->rect.y = PLAYER_INI_POS_Y;
  player->body->rect.w = PLAYER_SIZE_LIMB;
  player->body->rect.h = PLAYER_SIZE_LIMB;

  // Nesse for, settamos as posi��es e tamanhos dos restante do corpo das cobras
  /*int i = 1;
  for (i = 1; i < player->size + 1; i++) {
          player->body[i].rect.x = player->body[i - 1].rect.x - 40;
          player->body[i].rect.y = PLAYER_INI_POS_Y;
          player->body[i].rect.w = PLAYER_SIZE_LIMB;
          player->body[i].rect.h = PLAYER_SIZE_LIMB;
  }*/

  player->running = false;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error on initializing SDL: %s\n", SDL_GetError());
    return;
  }

  atexit(SDL_Quit);

  game->window = SDL_CreateWindow("Janela teste", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!game->window) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    return;
  }

  game->renderer =
      SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
}

// funcao para controlar os eventos
void controladorEvento(Game *game, Player *player) {
  // Event Handling
  SDL_Event e;
  while (SDL_PollEvent(&e)) {

    if (e.type == SDL_QUIT ||
        (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
      game->running = false;

    if (e.type == SDL_KEYDOWN) {

      switch (e.key.keysym.sym) {

      case SDLK_UP:
      case SDLK_w:
        if (player->body[0].currentDir == Bottom) {
          break;
        }
        player->running = true;
        player->nextDir = Top;
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        if (player->body[0].currentDir == Left) {
          break;
        }
        player->running = true;
        player->nextDir = Right;

        break;
      case SDLK_DOWN:
      case SDLK_s:
        if (player->body[0].currentDir == Top) {
          break;
        }
        player->running = true;
        player->nextDir = Bottom;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        if (player->body[0].currentDir == Right) {
          break;
        }
        player->running = true;
        player->nextDir = Left;
        break;
      }
    }
  }
}

/*
int validaDirecao(Direction dir1, Drection dir2) {

        if (dir1 == Top && dir2 == Bottom){}

}*/

void movimentoCobra(Player *player, Direction *direcaoDeMovimento) {
  Limb *player2 = player->body;
  int i;
  if (player->running) {

    /* condi��o que garante que a cabe�a esteja nos quadrados corretos para
     * fazer curvas e atualizar as dire��es
     */
    while (player2->prox != NULL)
      player2 = player2->prox;
    if (player->body->rect.y % 40 == 0 && player->body->rect.x % 40 == 0) {
      while (player2->ant != NULL) {
        /* no momento que um peda�o do corpo chegar a �ltima posi��o em que
         * esteve o pr�ximo peda�o do corpo, � atualizado a sua nova dire��o
         */
        if (player2->rect.x == player2->ant->lastValidCoor.x &&
            player2->rect.y == player2->ant->lastValidCoor.y) {
          player2->currentDir = player2->ant->currentDir;
        }

        /* dizemos que a ultima coordenada valida de cada peda�o do corpo � a
         * posi��o atual dele no momento. Esse valor j� estar� atualizado para a
         * proxima verifica��o.
         */
        player2->lastValidCoor.x = player2->rect.x;
        player2->lastValidCoor.y = player2->rect.y;
        player2 = player2->ant;
      }
      // atualiza~mos a dire��o da cabe�a de acordo com a ultima entrada pelo
      // teclado
      player->body[0].currentDir = player->nextDir;

      player->body[0].lastValidCoor.x = player->body[0].rect.x;
      player->body[0].lastValidCoor.y = player->body[0].rect.y;
    }
  }

  // para cada parte do corpo cabe�a, realizamos os seguintes testes
  player2 = player->body;
  while (player2 != NULL) {
    switch (player2->currentDir) {
    case Top:
      player2->rect.y -= 4;
      break;
    case Right:
      player2->rect.x += 4;
      break;
    case Bottom:
      player2->rect.y += 4;
      break;
    case Left:
      player2->rect.x -= 4;
      break;
    }
    player2 = player2->prox;
  }
}

// gera uma ma��o em uma posi��o aleat�ria com mesmo tamanho que o jogador
Maca *geraMaca(Player *player) {
  Limb *player2 = player->body;
  int teste = 1;
  int posx = (rand() % 16) * 40;
  int posy = (rand() % 16) * 40;
  while (teste) {
    teste = 0;
    while (player2->prox != NULL) {
      if (posx == player2->rect.x || posy == player2->rect.y) {
        posx = (rand() % 16) * 40;
        posy = (rand() % 16) * 40;
        teste = 1;
      }
      player2 = player2->prox;
    }
  }

  Maca *maca = (Maca *)malloc(sizeof(Maca));
  maca->rect.w = PLAYER_SIZE_LIMB;
  maca->rect.h = PLAYER_SIZE_LIMB;
  maca->rect.x = posx;
  maca->rect.y = posy;

  return maca;
}
// caso a cabe�a da cobra esteja na mesma posi��o da ma��, d� free e ent�o cria
// uma nova
Maca *checkaMaca(Player *player, Maca *maca) {
  if (player->body->rect.x == maca->rect.x &&
      player->body->rect.y == maca->rect.y) {
    free(maca);
    player->size++;
    Maca *macaNova = geraMaca(player);
    Limb *player2 = player->body;
    // pega o �ltimo membro da lista ligada
    while (player2->prox != NULL)
      player2 = player2->prox;

    // adicionando um membro depois disso
    player2->prox = (Limb *)malloc(sizeof(Limb));
    player2->prox->prox = NULL;
    player2->prox->ant = player2;
    player2->prox->rect.h = 40;
    player2->prox->rect.w = 40;
    player2->prox->currentDir = player2->currentDir;
    player2->prox->lastValidCoor = player2->lastValidCoor;

    switch (player2->currentDir) {
    case Top:
      player2->prox->rect.x = player2->rect.x;
      player2->prox->rect.y = player2->rect.y + 40;
      break;
    case Bottom:
      player2->prox->rect.x = player2->rect.x;
      player2->prox->rect.y = player2->rect.y - 40;
      break;
    case Left:
      player2->prox->rect.x = player2->rect.x + 40;
      player2->prox->rect.y = player2->rect.y;
      break;
    case Right:
      player2->prox->rect.x = player2->rect.x - 40;
      player2->prox->rect.y = player2->rect.y;
      break;
    }
    // fim da adi��o

    return macaNova; // retorna a nova ma��
  }
  return maca;
}

void drawPlayer(Game *game, Player *player, Maca *maca) {
  int i;
  // comandos para desenhar a cabe�a da cobra
  SDL_SetRenderDrawColor(game->renderer, 255, 100, 100, 255);
  SDL_RenderFillRect(game->renderer, &player->body->rect);
  Limb *player2;
  // desenha as outras partes do corpo com cor diferente daquela da cobra
  if (player->body->prox != NULL) {
    player2 = player->body->prox;
    while (player2 != NULL) {
      SDL_SetRenderDrawColor(game->renderer, 100, 100, 100, 255);
      SDL_RenderFillRect(game->renderer, &player2->rect);
      player2 = player2->prox;
    }
  }
  // desenha ma��
  SDL_SetRenderDrawColor(game->renderer, 50, 100, 100, 255);
  SDL_RenderFillRect(game->renderer, &maca->rect);
}

bool contato(Player *player) {
  Limb *player2 = player->body;

  if (player->running) {

    if (player2->prox)
      player2 = player2->prox;

    if (player->body->rect.y < 0 || player->body->rect.x < 0 ||
        player->body->rect.y > SCREEN_HEIGHT ||
        player->body->rect.x > SCREEN_WIDTH)
      return true;

    while (player2->prox != NULL) {
      int y1 = player->body->rect.y;
      int x1 = player->body->rect.x;
      int y2 = player2->rect.y;
      int x2 = player2->rect.x;

      if (player->body->currentDir == Bottom) {
        if ((y1 / PLAYER_SIZE_LIMB) + 1 == y2 / PLAYER_SIZE_LIMB &&
            x1 / PLAYER_SIZE_LIMB == (x2 + 4) / PLAYER_SIZE_LIMB)
          return true;
        player2 = player2->prox;
      } else if (player->body->currentDir == Top) {
        if ((y1 / PLAYER_SIZE_LIMB) == (y2 / PLAYER_SIZE_LIMB) &&
            x1 / PLAYER_SIZE_LIMB == (x2 + 4) / PLAYER_SIZE_LIMB)
          return true;
        player2 = player2->prox;
      } else if (player->body->currentDir == Left) {
        if (y1 / PLAYER_SIZE_LIMB == (y2 + 4) / PLAYER_SIZE_LIMB &&
            (x1 / PLAYER_SIZE_LIMB) == (x2) / PLAYER_SIZE_LIMB)
          return true;
        player2 = player2->prox;
      } else if (player->body->currentDir == Right) {
        if (y1 / PLAYER_SIZE_LIMB == (y2 + 4) / PLAYER_SIZE_LIMB &&
            (x1 / PLAYER_SIZE_LIMB) + 1 == (x2) / PLAYER_SIZE_LIMB)
          return true;
        player2 = player2->prox;
      }
    }
    return false;
  }
  return false;
}

// Game Loop
void gameLoop(Game *game, Player *player, Maca *maca) {

  while (game->running) {
    if (contato(player))
      game->running = false;
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderClear(game->renderer);

    controladorEvento(game, player);
    Direction direcaoDeMovimento;
    movimentoCobra(player, &direcaoDeMovimento);

    maca = checkaMaca(player, maca);

    drawPlayer(game, player, maca);

    // Screen update
    SDL_RenderPresent(game->renderer);
    SDL_Delay(1000 / 60);
  }
}

int main(int argc, char *argv[]) {
  Game *game = (Game *)malloc(sizeof(Game));
  Player *player = (Player *)malloc(sizeof(Player));
  inicializar(game, player);
  Maca *maca = geraMaca(player);
  gameLoop(game, player, maca);

  SDL_DestroyWindow(game->window);

  SDL_Quit();

  return 0;
}
