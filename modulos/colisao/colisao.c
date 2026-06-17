#include "colisao.h"
#include "../tipos.h"   /* ALTURA, HITBOX_W, HITBOX_H,
                             ZUMBI_HBX_OFFSET_X/Y, ZUMBI_HBX_W/H */
extern unsigned char colisao[ALTURA][3000];

bool pixel_solido(ALLEGRO_BITMAP *mapa, int x, int y)
{
    (void)mapa;
    if (x < 0 || y < 0 || x >= 3000 || y >= ALTURA)
        return true;
    return colisao[y][x];
}

bool colide_mapa(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int left = (int)x, right = (int)x + HITBOX_W - 1;
    int top  = (int)y, bottom = (int)y + HITBOX_H - 1;
    for (int px = left; px <= right; px += 4)
        if (pixel_solido(mapa, px, top))
            return true;
    for (int px = left; px <= right; px += 4)
        if (pixel_solido(mapa, px, bottom))
            return true;
    for (int py = top; py <= bottom; py += 4)
        if (pixel_solido(mapa, left, py))
            return true;
    for (int py = top; py <= bottom; py += 4)
        if (pixel_solido(mapa, right, py))
            return true;
    return false;
}

bool esta_no_chao(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int left = (int)x + 4, right = (int)x + HITBOX_W - 4;
    int foot = (int)y + HITBOX_H;
    return pixel_solido(mapa, left, foot) || pixel_solido(mapa, right, foot);
}

bool zumbi_colide_vertical(ALLEGRO_BITMAP *mapa, float x, float y, float vel_y)
{
    int hbx = (int)(x + ZUMBI_HBX_OFFSET_X), hbw = ZUMBI_HBX_W;
    if (vel_y > 0)
    {
        int foot = (int)(y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H);
        for (int px = hbx; px <= hbx + hbw; px += 6)
            if (pixel_solido(mapa, px, foot))
                return true;
    }
    else
    {
        int top = (int)(y + ZUMBI_HBX_OFFSET_Y);
        for (int px = hbx; px <= hbx + hbw; px += 6)
            if (pixel_solido(mapa, px, top))
                return true;
    }
    return false;
}

bool zumbi_no_chao(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx = (int)(x + ZUMBI_HBX_OFFSET_X), hbw = ZUMBI_HBX_W;
    int foot = (int)(y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H) + 2;
    for (int px = hbx; px <= hbx + hbw; px += 6)
        if (pixel_solido(mapa, px, foot))
            return true;
    return false;
}

bool zumbi_colide_horizontal(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx = (int)(x + ZUMBI_HBX_OFFSET_X);
    int hby = (int)(y + ZUMBI_HBX_OFFSET_Y);
    int hbh = ZUMBI_HBX_H;
    for (int py = hby; py <= hby + hbh; py += 6)
        if (pixel_solido(mapa, hbx, py) || pixel_solido(mapa, hbx + ZUMBI_HBX_W, py))
            return true;
    return false;
}