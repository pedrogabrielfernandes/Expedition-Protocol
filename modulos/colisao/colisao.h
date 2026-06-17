#ifndef COLISAO_H
#define COLISAO_H

#include <allegro5/allegro.h>
#include <stdbool.h>

bool pixel_solido          (ALLEGRO_BITMAP *mapa, int x, int y);
bool colide_mapa           (ALLEGRO_BITMAP *mapa, float x, float y);
bool esta_no_chao          (ALLEGRO_BITMAP *mapa, float x, float y);
bool zumbi_colide_vertical (ALLEGRO_BITMAP *mapa, float x, float y, float vel_y);
bool zumbi_no_chao         (ALLEGRO_BITMAP *mapa, float x, float y);
bool zumbi_colide_horizontal(ALLEGRO_BITMAP *mapa, float x, float y);

#endif /* COLISAO_H */