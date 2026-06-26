#ifndef GAME_OVER_H
#define GAME_OVER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "../../tipos.h"
#include "../../sistemas/sons/sons.h"

int tela_game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                   ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                   const char *motivo, Sons *sons);

#endif