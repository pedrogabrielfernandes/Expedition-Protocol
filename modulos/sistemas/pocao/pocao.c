#include "pocao.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#include "../vida/vida.h"
#include "../../tipos.h"
#include "../colisao/colisao.h"
#include "../sons/sons.h"
#include "../../mapa/mapa.h"   /* ConfigMapa, mapa_obter */

/* ================================================================== */
/*  POÇÃO                                                               */
/* ================================================================== */
void pocao_tentar_spawn(Pocao *p, VidaStatus *vidas, int mapa_id)
{
    if (p->ativa || contar_vidas(vidas) != 1)
        return;

    const ConfigMapa *cfg = mapa_obter(mapa_id);

    if (cfg && cfg->pocao_spawn_fixo)
    {
        /* Posição fixa: não cai, fica exatamente onde foi configurado */
        p->x       = cfg->pocao_spawn_x;
        p->y       = cfg->pocao_spawn_y;
        p->vel_y   = 0.0f;
        p->no_chao = 0;
    }
    else
    {
        /* Posição aleatória, cai pela gravidade até o piso */
        p->x       = (float)(POCAO_SPAWN_X_MIN + rand() % (POCAO_SPAWN_X_MAX - POCAO_SPAWN_X_MIN));
        p->y       = (float)POCAO_SPAWN_Y;
        p->vel_y   = 0.0f;
        p->no_chao = 0;
    }

    p->ativa    = 1;
    p->coletada = 0;
}

static bool pocao_piso_abaixo(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int foot  = (int)(y + POCAO_ALTURA) + 1;
    int left  = (int)(x + (POCAO_LARGURA - POCAO_HITBOX_W) / 2.0f);
    int right = left + POCAO_HITBOX_W - 1;

    for (int px2 = left; px2 <= right; px2 += 4)
        if (pixel_solido(mapa, px2, foot))
            return true;

    return false;
}

void pocao_atualizar(Pocao *p, Jogador *jog, VidaStatus *vidas, Sons *sons)
{
    if (!p->ativa)
        return;

    ALLEGRO_BITMAP *mapa = get_jog_mapa();

    if (!p->no_chao)
    {
        p->vel_y += POCAO_GRAVIDADE;

        if (p->vel_y > POCAO_MAX_QUEDA)
            p->vel_y = POCAO_MAX_QUEDA;

        float ny = p->y + p->vel_y;

        if (pocao_piso_abaixo(mapa, p->x, ny))
        {
            while (!pocao_piso_abaixo(mapa, p->x, p->y + 1.0f))
                p->y += 1.0f;

            p->vel_y   = 0.0f;
            p->no_chao = 0;
        }
        else
        {
            p->y = ny;
        }

        if (p->y > ALTURA + 100)
        {
            p->ativa = 0;
            return;
        }
    }

    if (p->x < jog->mov.x + HITBOX_W  && p->x + POCAO_HITBOX_W > jog->mov.x &&
        p->y < jog->mov.y + HITBOX_H  && p->y + POCAO_HITBOX_H > jog->mov.y)
    {
        restaurar_vidas(vidas, POCAO_RECUPERA_VIDAS);
        tocar(sons->pocao_som);
        p->ativa    = 0;
        p->coletada = 1;
    }
}

void pocao_desenhar(Pocao *p, ALLEGRO_BITMAP *spr)
{
    if (!p->ativa || !spr)
        return;

    double t      = al_get_time();
    float  brilho = 0.5f + 0.5f * sinf((float)t * 4.0f);
    unsigned char ga = (unsigned char)(60.0f + 80.0f * brilho);

    al_draw_filled_circle(
        p->x + POCAO_LARGURA / 2.0f,
        p->y + POCAO_ALTURA  / 2.0f,
        POCAO_LARGURA * 0.7f,
        al_map_rgba(100, 220, 100, ga)
    );

    al_draw_scaled_bitmap(
        spr,
        0, 0,
        al_get_bitmap_width(spr),
        al_get_bitmap_height(spr),
        p->x, p->y,
        POCAO_LARGURA, POCAO_ALTURA,
        0
    );
}