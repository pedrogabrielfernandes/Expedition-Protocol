#include "samurai.h"
#include <math.h>

/* ================================================================== */
/*  DESENHO DO SAMURAI                                                */
/* ================================================================== */
void desenhar_samurai(Jogador *jog, SamuraiSprites *spr)
{
    float draw_x = jog->mov.x - HITBOX_OFFSET_X;
    float draw_y = jog->mov.y - HITBOX_OFFSET_Y;

    switch (jog->estado)
    {
    case SAM_HURT:
    {
        double elapsed = al_get_time() - jog->hurt_inicio;
        int f = (int)((elapsed / KNOCKBACK_SAMURAI_DURACAO) * FRAMES_HURT);
        if (f >= FRAMES_HURT) f = FRAMES_HURT - 1;

        al_draw_scaled_bitmap(spr->hurt, 128 * f, 0, 128, 128,
                              draw_x, draw_y, DRAW_W, DRAW_H,
                              jog->direcao);

        if (elapsed >= KNOCKBACK_SAMURAI_DURACAO)
        {
            jog->estado = SAM_IDLE;
            jog->frame_hurt = 0;
        }
        break;
    }

    case SAM_DEAD:
    {
        int f = (int)jog->frame_dead;
        if (f >= FRAMES_DEAD)
        {
            f = FRAMES_DEAD - 1;
            jog->morte_animando = 0;
        }
        else
            jog->morte_animando = 1;

        al_draw_scaled_bitmap(spr->dead, 128 * f, 0, 128, 128,
                              draw_x, draw_y, DRAW_W, DRAW_H,
                              jog->direcao);

        jog->frame_dead += 0.06f;
        break;
    }

    case SAM_ATTACK:
    {
        int mf;
        if (jog->tipo_ataque == 1)
            mf = 6;
        else if (jog->tipo_ataque == 2)
            mf = 4;
        else
            mf = 3;

        int f = (int)jog->frame_ataque;
        if (f >= mf) f = mf - 1;

        ALLEGRO_BITMAP *atk;
        if (jog->tipo_ataque == 1)
            atk = spr->attack1;
        else if (jog->tipo_ataque == 2)
            atk = spr->attack2;
        else
            atk = spr->attack3;

        al_draw_scaled_bitmap(atk, 128 * f, 0, 128, 128,
                              draw_x - 10, draw_y - 1,
                              ATTACK_DRAW_W, ATTACK_DRAW_H,
                              jog->direcao);

        jog->frame_ataque += 0.30f;

        if (jog->frame_ataque >= mf)
        {
            jog->atacando = 0;
            jog->frame_ataque = 0;
            jog->estado = SAM_IDLE;
            jog->som_ataque_tocado = 0;
            jog->dash_som_tocado = 0;
        }
        break;
    }

    case SAM_DASH:
    {
        int f = (int)jog->dash_fuga_frame % FRAMES_RUN;

        float off;
        if (jog->direcao == 0)
            off = -18.0f;
        else
            off = 18.0f;

        al_draw_tinted_scaled_bitmap(spr->run,
            al_map_rgba(180, 240, 255, 220),
            128 * f, 0, 128, 128,
            draw_x, draw_y, DRAW_W, DRAW_H,
            jog->direcao);

        al_draw_tinted_scaled_bitmap(spr->run,
            al_map_rgba(100, 200, 255, 80),
            128 * f, 0, 128, 128,
            draw_x + off, draw_y + 4,
            DRAW_W, DRAW_H,
            jog->direcao);

        al_draw_tinted_scaled_bitmap(spr->run,
            al_map_rgba(100, 200, 255, 40),
            128 * f, 0, 128, 128,
            draw_x + off * 2.0f, draw_y + 8,
            DRAW_W, DRAW_H,
            jog->direcao);

        jog->dash_fuga_frame += 0.5f;
        break;
    }

    case SAM_CHARGING:
    {
        int f = (int)jog->frame % FRAMES_IDLE;
        float intensidade = 0.6f + 0.8f * jog->carga_atk2_pct;
        float shake_x = sinf((float)al_get_time() * 30.0f) * intensidade;

        al_draw_scaled_bitmap(spr->idle, 128 * f, 0, 128, 128,
                              draw_x + shake_x, draw_y,
                              DRAW_W, DRAW_H,
                              jog->direcao);
        break;
    }

    case SAM_JUMP:
    {
        int f = (int)jog->frame % FRAMES_JUMP;

        al_draw_scaled_bitmap(spr->jump, 128 * f, 0, 128, 128,
                              draw_x, draw_y,
                              DRAW_W, DRAW_H,
                              jog->direcao);
        break;
    }

    case SAM_RUN:
    {
        int f = (int)jog->frame % FRAMES_RUN;

        al_draw_scaled_bitmap(spr->run, 128 * f, 0, 128, 128,
                              draw_x, draw_y,
                              DRAW_W, DRAW_H,
                              jog->direcao);
        break;
    }

    default:
    {
        int f = (int)jog->frame % FRAMES_IDLE;

        al_draw_scaled_bitmap(spr->idle, 128 * f, 0, 128, 128,
                              draw_x, draw_y,
                              DRAW_W, DRAW_H,
                              jog->direcao);
        break;
    }
    }
}

/* ================================================================== */
/*  ESTADO DO SAMURAI                                                 */
/* ================================================================== */
void atualizar_estado_samurai(Jogador *jog)
{
    if (jog->estado == SAM_DEAD || jog->estado == SAM_HURT ||
        jog->estado == SAM_DASH || jog->estado == SAM_CHARGING)
        return;

    if (jog->atacando)
    {
        jog->estado = SAM_ATTACK;
        return;
    }

    if (!jog->no_chao)
        jog->estado = SAM_JUMP;
    else if (jog->movendo)
        jog->estado = SAM_RUN;
    else
        jog->estado = SAM_IDLE;
}