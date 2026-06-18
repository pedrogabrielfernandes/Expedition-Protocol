#ifndef INTERFACE_H
#define INTERFACE_H

#include "../tipos.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>


OpcaoMenu executar_menu(
    ALLEGRO_EVENT_QUEUE *queue,
    ALLEGRO_TIMER *timer,
    ALLEGRO_BITMAP *bg_menu,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_hud,
    Sons *sons
);


void desenhar_vidas(
    VidaStatus *vidas,
    ALLEGRO_BITMAP *coracao
);


void desenhar_estamina(
    float estamina,
    ALLEGRO_BITMAP *spr
);


void desenhar_sanidade(
    Sanidade *san,
    ALLEGRO_BITMAP *spr
);


void desenhar_hud_tempo(
    ALLEGRO_FONT *f,
    const char *txt
);


void desenhar_hud_horda(
    ALLEGRO_FONT *f,
    Horda *h
);


void desenhar_hud_ataque(
    ALLEGRO_BITMAP *at1,
    ALLEGRO_BITMAP *at2,
    ALLEGRO_BITMAP *at3,
    ALLEGRO_BITMAP *hab1,
    ALLEGRO_BITMAP *hab2,
    ALLEGRO_BITMAP *hab3,
    int selecao,
    ALLEGRO_FONT *fonte_hud
);


void desenhar_hud_dash_fuga(
    Jogador *jog,
    ALLEGRO_FONT *fonte_hud
);


void desenhar_hud_carga_atk2(
    Jogador *jog,
    ALLEGRO_FONT *fonte_hud
);


void desenhar_roda_habilidade(
    ALLEGRO_BITMAP *at1,
    ALLEGRO_BITMAP *at2,
    ALLEGRO_BITMAP *at3,
    ALLEGRO_BITMAP *hab1,
    ALLEGRO_BITMAP *hab2,
    ALLEGRO_BITMAP *hab3,
    int selecao,
    ALLEGRO_FONT *fonte_hud
);


#endif