#ifndef SANIDADE_H
#define SANIDADE_H

#include "../../tipos.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>


void atualizar_sanidade(Sanidade *san);

void desenhar_overlay_sanidade(Sanidade *san);

void desenhar_aviso_sanidade(
    Sanidade *san,
    ALLEGRO_FONT *fonte,
    double t
);

void desenhar_mensagem_central_insanidade(
    Sanidade *san,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_hud,
    double t
);


#endif