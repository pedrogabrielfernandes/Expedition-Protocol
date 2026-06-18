#ifndef RANKING_H
#define RANKING_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <math.h>

#include "../../tipos.h"

void ordenar_ranking(float r[], int n);
int busca_binaria(float r[], int n, float v);

void carregar_ranking(Temporizador *t);
void salvar_ranking(Temporizador *t);

void tela_ranking(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                  ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                  Temporizador *tempo, double tempo_final);

#endif