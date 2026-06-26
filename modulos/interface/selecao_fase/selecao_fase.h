#ifndef SELECAO_FASE_H
#define SELECAO_FASE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "../../sistemas/sons/sons.h"

/* ================================================================== */
/*  Progresso persistente do jogador                                  */
/* ================================================================== */
#define PROGRESSO_ARQUIVO   "saves/progresso.dat"
#define PROGRESSO_MAX_FASES 16

typedef struct {
    int fases_desbloqueadas;
    int ultima_fase_jogada;
} ProgressoJogo;

void progresso_carregar(ProgressoJogo *prog);
void progresso_avancar_fase(ProgressoJogo *prog, int fase_idx);

/* ================================================================== */
/*  Telas                                                             */
/* ================================================================== */
int tela_selecao_fase(ALLEGRO_EVENT_QUEUE *queue,
                      ALLEGRO_TIMER       *timer,
                      ALLEGRO_FONT        *fonte,
                      ALLEGRO_FONT        *fonte_hud,
                      ProgressoJogo       *prog,
                      Sons                *sons);

void tela_proxima_fase(ALLEGRO_EVENT_QUEUE *queue,
                       ALLEGRO_TIMER       *timer,
                       ALLEGRO_FONT        *fonte,
                       ALLEGRO_FONT        *fonte_hud,
                       int                  fase_atual,
                       int                  proximo_idx,
                       const void          *cfg_prox);

#endif /* SELECAO_FASE_H */