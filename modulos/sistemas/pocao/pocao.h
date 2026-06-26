#ifndef POCAO_H
#define POCAO_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "../../tipos.h"

ALLEGRO_BITMAP *get_jog_mapa(void);
void pocao_tentar_spawn(Pocao *p, VidaStatus *vidas, int mapa_id);
void pocao_atualizar(Pocao *p, Jogador *jog, VidaStatus *vidas, Sons *sons);
void pocao_desenhar(Pocao *p, ALLEGRO_BITMAP *spr);

#endif