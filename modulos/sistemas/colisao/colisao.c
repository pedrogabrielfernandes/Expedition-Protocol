#include "colisao.h"
#include "../../tipos.h"
#include "../../entidades/boss/boss.h"

extern unsigned char colisao[ALTURA][3000];

void gerar_mapa_colisao(ALLEGRO_BITMAP *mapa)
{
    al_lock_bitmap(mapa, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
    for (int y = 0; y < al_get_bitmap_height(mapa); y++)
    {
        for (int x = 0; x < al_get_bitmap_width(mapa); x++)
        {
            ALLEGRO_COLOR c = al_get_pixel(mapa, x, y);
            unsigned char r, g, b;
            al_unmap_rgb(c, &r, &g, &b);
            colisao[y][x] = ((r + g + b) / 3 < 120);
        }
    }
    al_unlock_bitmap(mapa);
}

bool pixel_solido(ALLEGRO_BITMAP *mapa, int x, int y)
{
    (void)mapa;
    if (x < 0 || y < 0 || x >= 3000 || y >= ALTURA)
        return true;
    return colisao[y][x];
}

bool colide_mapa(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int left   = (int)x;
    int right  = (int)x + HITBOX_W - 1;
    int top    = (int)y;
    int bottom = (int)y + HITBOX_H - 1;

    for (int px = left; px <= right; px += 4)
        if (pixel_solido(mapa, px, top))    return true;
    for (int px = left; px <= right; px += 4)
        if (pixel_solido(mapa, px, bottom)) return true;
    for (int py = top; py <= bottom; py += 4)
        if (pixel_solido(mapa, left,  py))  return true;
    for (int py = top; py <= bottom; py += 4)
        if (pixel_solido(mapa, right, py))  return true;
    return false;
}

bool esta_no_chao(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int left  = (int)x + 4;
    int right = (int)x + HITBOX_W - 4;
    int foot  = (int)y + HITBOX_H;
    return pixel_solido(mapa, left, foot) ||
           pixel_solido(mapa, right, foot);
}

bool zumbi_colide_vertical(ALLEGRO_BITMAP *mapa, float x, float y, float vel_y)
{
    int hbx = (int)(x + ZUMBI_HBX_OFFSET_X);
    int hbw = ZUMBI_HBX_W;

    if (vel_y > 0)
    {
        int foot = (int)(y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H);
        for (int px = hbx; px <= hbx + hbw; px += 6)
            if (pixel_solido(mapa, px, foot)) return true;
    }
    else
    {
        int top = (int)(y + ZUMBI_HBX_OFFSET_Y);
        for (int px = hbx; px <= hbx + hbw; px += 6)
            if (pixel_solido(mapa, px, top)) return true;
    }
    return false;
}

bool zumbi_no_chao(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx  = (int)(x + ZUMBI_HBX_OFFSET_X);
    int hbw  = ZUMBI_HBX_W;
    int foot = (int)(y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H) + 2;
    for (int px = hbx; px <= hbx + hbw; px += 6)
        if (pixel_solido(mapa, px, foot)) return true;
    return false;
}

bool zumbi_colide_horizontal(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx = (int)(x + ZUMBI_HBX_OFFSET_X);
    int hby = (int)(y + ZUMBI_HBX_OFFSET_Y);
    int hbh = ZUMBI_HBX_H;
    for (int py = hby; py <= hby + hbh; py += 6)
        if (pixel_solido(mapa, hbx, py) ||
            pixel_solido(mapa, hbx + ZUMBI_HBX_W, py))
            return true;
    return false;
}

/* ------------------------------------------------------------------ */
/*  Boss  vertical                                                     */
/*  BUG CORRIGIDO: era >= 0, bloqueava queda quando vel_y == 0         */
/* ------------------------------------------------------------------ */
bool boss_colide_vertical(ALLEGRO_BITMAP *mapa, float x, float y, float vel_y)
{
    int hbx = (int)(x + BOSS_HBX_OFFSET_X);
    int hbw = (int)BOSS_HBX_W;

    if (vel_y > 0)
    {
        int foot = (int)(y + BOSS_HBX_OFFSET_Y + BOSS_HBX_H);
        for (int px = hbx; px <= hbx + hbw; px += 8)
            if (pixel_solido(mapa, px, foot)) return true;
    }
    else if (vel_y < 0)
    {
        int top = (int)(y + BOSS_HBX_OFFSET_Y);
        for (int px = hbx; px <= hbx + hbw; px += 8)
            if (pixel_solido(mapa, px, top)) return true;
    }
    return false;
}

bool boss_no_chao(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx  = (int)(x + BOSS_HBX_OFFSET_X);
    int hbw  = (int)BOSS_HBX_W;
    int foot = (int)(y + BOSS_HBX_OFFSET_Y + BOSS_HBX_H) + 2;
    for (int px = hbx; px <= hbx + hbw; px += 8)
        if (pixel_solido(mapa, px, foot)) return true;
    return false;
}

/* ------------------------------------------------------------------ */
/*  Boss  horizontal (NOVO)                                            */
/*  Usada no BOSS_WALK para checar paredes laterais                    */
/* ------------------------------------------------------------------ */
bool boss_colide_horizontal(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int hbx = (int)(x + BOSS_HBX_OFFSET_X);
    int hby = (int)(y + BOSS_HBX_OFFSET_Y);
    int hbh = (int)BOSS_HBX_H;

    for (int py = hby; py <= hby + hbh; py += 8)
        if (pixel_solido(mapa, hbx, py) ||
            pixel_solido(mapa, hbx + (int)BOSS_HBX_W, py))
            return true;
    return false;
}