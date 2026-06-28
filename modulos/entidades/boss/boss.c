#include "boss.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

extern bool pixel_solido(ALLEGRO_BITMAP *mapa, int x, int y);
extern bool boss_colide_vertical(ALLEGRO_BITMAP *mapa, float x, float y, float vel_y);
extern bool boss_colide_horizontal(ALLEGRO_BITMAP *mapa, float x, float y);
extern bool boss_no_chao(ALLEGRO_BITMAP *mapa, float x, float y);
extern void perder_vida(VidaStatus *vs);

/* ------------------------------------------------------------------ */
/*  montar_tira                                                         */
/* ------------------------------------------------------------------ */
static ALLEGRO_BITMAP *montar_tira(const char *pasta, const char *nome,
                                   int n_frames)
{
    int tira_w = BOSS_SPRITE_W * n_frames;
    int tira_h = BOSS_SPRITE_H;

    ALLEGRO_BITMAP *tira = al_create_bitmap(tira_w, tira_h);
    if (!tira)
        return NULL;

    ALLEGRO_BITMAP *alvo_anterior = al_get_target_bitmap();
    al_set_target_bitmap(tira);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    for (int i = 0; i < n_frames; i++)
    {
        char caminho[128];
        snprintf(caminho, sizeof(caminho),
                 "assets/sprites/boss/%s/%s%d.png",
                 pasta, nome, i + 1);

        ALLEGRO_BITMAP *frame = al_load_bitmap(caminho);
        if (!frame)
        {
            printf("Boss: nao encontrou %s\n", caminho);
            al_set_target_bitmap(alvo_anterior);
            al_destroy_bitmap(tira);
            return NULL;
        }

        int fw = al_get_bitmap_width(frame);
        int fh = al_get_bitmap_height(frame);
        if (fw != BOSS_SPRITE_W || fh != BOSS_SPRITE_H)
        {
            ALLEGRO_BITMAP *escalado = al_create_bitmap(BOSS_SPRITE_W, BOSS_SPRITE_H);
            ALLEGRO_BITMAP *prev2 = al_get_target_bitmap();
            al_set_target_bitmap(escalado);
            al_clear_to_color(al_map_rgba(0, 0, 0, 0));
            al_draw_scaled_bitmap(frame, 0, 0, fw, fh,
                                  0, 0, BOSS_SPRITE_W, BOSS_SPRITE_H, 0);
            al_set_target_bitmap(prev2);
            al_destroy_bitmap(frame);
            frame = escalado;
        }

        al_draw_bitmap(frame, i * BOSS_SPRITE_W, 0, 0);
        al_destroy_bitmap(frame);
    }

    al_set_target_bitmap(alvo_anterior);
    return tira;
}

/* ================================================================== */
/*  boss_init                                                           */
/* ================================================================== */
void boss_init(Boss *b, float x, float y)
{
    memset(b, 0, sizeof(Boss));

    b->x = x;
    b->y = y;
    b->vel_y = 0.0f;
    b->direcao = 1;
    b->estado = BOSS_IDLE;
    b->fase = 1;
    b->hp = BOSS_HP_MAX;
    b->hp_max = BOSS_HP_MAX;
    b->ativo = 1;
    b->no_chao = 1;
    b->dano_aplicado = 0;

    b->ultimo_ataque = -999.0;
    b->hurt_inicio = -999.0;
    b->ultimo_dano_recebido = -999.0;

    b->morte_animando = 0;
    b->som_morte_tocado = 0;
    b->som_spawn_tocado = 0;
    b->som_fase2_tocado = 0;
    b->som_fase3_tocado = 0;
}

/* ================================================================== */
/*  boss_sprites_carregar / destruir                                    */
/* ================================================================== */
int boss_sprites_carregar(BossSprites *spr)
{
    spr->idle   = montar_tira("idle",   "idle",   BOSS_FRAMES_IDLE);
    spr->walk   = montar_tira("walk",   "walk",   BOSS_FRAMES_WALK);
    spr->attack = montar_tira("attack", "attack", BOSS_FRAMES_ATTACK);
    spr->damage = montar_tira("damage", "damage", BOSS_FRAMES_DAMAGE);
    spr->death  = montar_tira("death",  "death",  BOSS_FRAMES_DEATH);

    return (spr->idle && spr->walk && spr->attack &&
            spr->damage && spr->death);
}

void boss_sprites_destruir(BossSprites *spr)
{
    if (spr->idle)   { al_destroy_bitmap(spr->idle);   spr->idle   = NULL; }
    if (spr->walk)   { al_destroy_bitmap(spr->walk);   spr->walk   = NULL; }
    if (spr->attack) { al_destroy_bitmap(spr->attack); spr->attack = NULL; }
    if (spr->damage) { al_destroy_bitmap(spr->damage); spr->damage = NULL; }
    if (spr->death)  { al_destroy_bitmap(spr->death);  spr->death  = NULL; }
}

/* ================================================================== */
/*  Helpers de hitbox                                                   */
/* ================================================================== */
static int boss_esta_espelhado(const Boss *b)
{
#if BOSS_SPRITE_OLHA_DIREITA
    return (b->direcao == 1);
#else
    return (b->direcao == 0);
#endif
}

/*
 * Offset da hitbox corrigido por direção.
 *
 * O sprite original olha para a ESQUERDA (BOSS_SPRITE_OLHA_DIREITA=0).
 * Quando espelhado (olhando direita), o offset precisa ser calculado
 * como o espelho do offset original dentro do sprite escalado.
 *
 * esquerda : BOSS_HBX_OFFSET_X
 * direita  : largura_tela - BOSS_HBX_OFFSET_X - BOSS_HBX_W
 */
static float boss_hbx_offset_x(const Boss *b)
{
    if (boss_esta_espelhado(b))
        return (BOSS_SPRITE_W * BOSS_ESCALA) - BOSS_HBX_OFFSET_X - BOSS_HBX_W;

    return BOSS_HBX_OFFSET_X;
}

static int hitboxes_sobrepoem(const Boss *b, const Jogador *j)
{
    float bx1 = b->x + boss_hbx_offset_x(b),
          bx2 = bx1 + BOSS_HBX_W;
    float by1 = b->y + BOSS_HBX_OFFSET_Y,
          by2 = by1 + BOSS_HBX_H;
    float jx1 = j->mov.x + HITBOX_OFFSET_X,
          jx2 = jx1 + HITBOX_W;
    float jy1 = j->mov.y + HITBOX_OFFSET_Y,
          jy2 = jy1 + HITBOX_H;

    return (bx1 < jx2 && bx2 > jx1 &&
            by1 < jy2 && by2 > jy1);
}

static float dist_bordas(const Boss *b, const Jogador *j)
{
    float bbx1 = b->x + boss_hbx_offset_x(b);
    float bbx2 = bbx1 + BOSS_HBX_W;
    float jbx1 = j->mov.x + HITBOX_OFFSET_X;
    float jbx2 = jbx1 + HITBOX_W;

    if (jbx1 > bbx2)
        return jbx1 - bbx2;
    else if (jbx2 < bbx1)
        return bbx1 - jbx2;
    else
        return 0.0f;
}

static void boss_olhar_jogador(Boss *b, const Jogador *jogador)
{
    float jog_cx = jogador->mov.x + HITBOX_OFFSET_X + HITBOX_W / 2.0f;
    float bos_cx = b->x + (BOSS_SPRITE_W * BOSS_ESCALA) / 2.0f;

    if (jog_cx > bos_cx)
        b->direcao = 0;
    else
        b->direcao = 1;
}

static void tocar_fase2(Boss *b, Sons *sons)
{
    if (!b->som_fase2_tocado && sons)
    {
        tocar(sons->boss_furioso);
        b->som_fase2_tocado = 1;
    }
}

static void tocar_fase3(Boss *b, Sons *sons)
{
    if (!b->som_fase3_tocado && sons)
    {
        tocar(sons->boss_furioso);
        b->som_fase3_tocado = 1;
    }
}

/* ================================================================== */
/*  boss_atualizar                                                      */
/* ================================================================== */
void boss_atualizar(Boss *b, Jogador *jogador, Sons *sons,
                    ALLEGRO_BITMAP *mapa_colisao, VidaStatus *vidas)
{
    if (!b->ativo)
        return;

    double agora = al_get_time();

    /* Transição de fase */
    {
        float pct_hp = (float)b->hp / (float)b->hp_max;
        if (pct_hp <= BOSS_FASE3_HP_PCT && b->fase < 3)
        {
            b->fase = 3;
            tocar_fase3(b, sons);
        }
        else if (pct_hp <= BOSS_FASE2_HP_PCT && b->fase < 2)
        {
            b->fase = 2;
            tocar_fase2(b, sons);
        }
    }

    float vel_atual;
    if (b->fase == 3)
        vel_atual = BOSS_VEL_FASE3;
    else if (b->fase == 2)
        vel_atual = BOSS_VEL_FASE2;
    else
        vel_atual = BOSS_VELOCIDADE;

    /* ---- Máquina de estados ---- */
    switch (b->estado)
    {
    /* ---- IDLE ---- */
    case BOSS_IDLE:
    {
        boss_olhar_jogador(b, jogador);

        b->frame += BOSS_ANIM_IDLE_VEL;
        if (b->frame >= BOSS_FRAMES_IDLE)
        {
            b->frame = 0.0f;

            float dist = dist_bordas(b, jogador);

            if (dist <= BOSS_ALCANCE_ATAQUE &&
                (agora - b->ultimo_ataque) >= BOSS_DELAY_ATAQUE)
            {
                b->estado        = BOSS_ATTACK;
                b->frame         = 0.0f;
                b->dano_aplicado = 0;
                b->ultimo_ataque = agora;
                if (sons) tocar(sons->boss_ataque);
            }
            else
            {
                b->estado = BOSS_WALK;
            }
        }
        break;
    }

    /* ---- WALK ---- */
    case BOSS_WALK:
    {
        b->frame += BOSS_ANIM_WALK_VEL;
        if (b->frame >= BOSS_FRAMES_WALK)
            b->frame = 0.0f;

        boss_olhar_jogador(b, jogador);

        float dist = dist_bordas(b, jogador);

        /* Jogador no alcance e cooldown zerado -> atacar */
        if (dist <= BOSS_ALCANCE_ATAQUE &&
            (agora - b->ultimo_ataque) >= BOSS_DELAY_ATAQUE)
        {
            b->estado        = BOSS_ATTACK;
            b->frame         = 0.0f;
            b->dano_aplicado = 0;
            b->ultimo_ataque = agora;
            if (sons) tocar(sons->boss_ataque);
            break;
        }

        float jog_cx = jogador->mov.x + HITBOX_OFFSET_X + HITBOX_W / 2.0f;
        float bos_cx = b->x + (BOSS_SPRITE_W * BOSS_ESCALA) / 2.0f;

        /* Distância ideal: meio entre mínima e alcance de ataque */
        float dist_ideal = (BOSS_DIST_MINIMA + BOSS_ALCANCE_ATAQUE) / 2.0f;

        if (dist < BOSS_DIST_MINIMA)
        {
            /* Muito perto: recua para abrir espaço */
            float nx;
            if (jog_cx > bos_cx)
                nx = b->x - vel_atual;
            else
                nx = b->x + vel_atual;

            if (!boss_colide_horizontal(mapa_colisao, nx, b->y))
                b->x = nx;
        }
        else if (dist > BOSS_ALCANCE_ATAQUE)
        {
            /* Longe demais: avança em direção ao jogador */
            if (jog_cx > bos_cx)
            {
                float nx = b->x + vel_atual;
                if (!boss_colide_horizontal(mapa_colisao, nx, b->y))
                    b->x = nx;
            }
            else
            {
                float nx = b->x - vel_atual;
                if (!boss_colide_horizontal(mapa_colisao, nx, b->y))
                    b->x = nx;
            }
        }
        else if (dist > dist_ideal)
        {
            /* Entre ideal e alcance: avança devagar para ficar na distância certa */
            float nx;
            if (jog_cx > bos_cx)
                nx = b->x + vel_atual * 0.5f;
            else
                nx = b->x - vel_atual * 0.5f;

            if (!boss_colide_horizontal(mapa_colisao, nx, b->y))
                b->x = nx;
        }
        /* Entre minima e ideal: fica parado esperando cooldown */
        break;
    }

    /* ---- ATTACK ---- */
    case BOSS_ATTACK:
    {
        boss_olhar_jogador(b, jogador);

        b->frame += BOSS_ANIM_ATAQUE_VEL;

        if (!b->dano_aplicado && (int)b->frame >= 8)
        {
            if (sons)
            {
                if (hitboxes_sobrepoem(b, jogador))
                    tocar(sons->boss_espada);
                else
                    tocar(sons->boss_ataque_errou);
            }
            boss_verificar_dano_jogador(b, jogador, vidas, sons);
            b->dano_aplicado = 1;
        }

        if (b->frame >= BOSS_FRAMES_ATTACK)
        {
            b->frame  = 0.0f;
            b->estado = BOSS_WALK;
        }
        break;
    }

    /* ---- DAMAGE ---- */
    case BOSS_DAMAGE:
        b->frame += BOSS_ANIM_DANO_VEL;
        if (b->frame >= BOSS_FRAMES_DAMAGE)
        {
            b->frame = 0.0f;
            if (b->hp > 0)
                b->estado = BOSS_WALK;
            else
                b->estado = BOSS_DEATH;

            if (b->estado == BOSS_DEATH)
                b->morte_animando = 1;
        }
        break;

    /* ---- DEATH ---- */
    case BOSS_DEATH:
        b->frame += BOSS_ANIM_DEATH_VEL;
        if (!b->som_morte_tocado && sons)
        {
            tocar(sons->boss_morte);
            b->som_morte_tocado = 1;
        }
        if (b->frame >= BOSS_FRAMES_DEATH)
        {
            b->frame          = (float)(BOSS_FRAMES_DEATH - 1);
            b->morte_animando = 0;
            b->estado         = BOSS_MORTO;
        }
        break;

    case BOSS_MORTO:
        break;

    default:
        b->estado = BOSS_IDLE;
        break;
    }

    /* ---- Fisica vertical ---- */
    if (b->estado != BOSS_MORTO)
    {
        b->vel_y += GRAVIDADE;
        if (b->vel_y > MAX_QUEDA)
            b->vel_y = MAX_QUEDA;

        b->y += b->vel_y;

        if (boss_no_chao(mapa_colisao, b->x, b->y))
        {
            while (boss_no_chao(mapa_colisao, b->x, b->y))
                b->y -= 1.0f;
            b->y    += 1.0f;
            b->vel_y = 0.0f;
        }

        b->no_chao = boss_no_chao(mapa_colisao, b->x, b->y);
    }
}

/* ================================================================== */
/*  boss_receber_dano                                                   */
/* ================================================================== */
int boss_receber_dano(Boss *b, int dano, Sons *sons)
{
    if (!b->ativo ||
        b->estado == BOSS_DEATH || b->estado == BOSS_MORTO)
        return 0;

    double agora = al_get_time();

    if ((agora - b->ultimo_dano_recebido) < 0.8)
        return 0;

    b->ultimo_dano_recebido = agora;
    b->hp -= dano;
    if (b->hp < 0)
        b->hp = 0;

    if (sons) tocar(sons->boss_dano);

    if (b->hp <= 0)
    {
        b->estado         = BOSS_DEATH;
        b->frame          = 0.0f;
        b->morte_animando = 1;
        return 1;
    }

    b->estado      = BOSS_DAMAGE;
    b->frame       = 0.0f;
    b->hurt_inicio = agora;

    return 0;
}

/* ================================================================== */
/*  boss_verificar_dano_jogador                                         */
/* ================================================================== */
void boss_verificar_dano_jogador(Boss *b, Jogador *jogador,
                                 VidaStatus *vidas, Sons *sons)
{
    if (!b->ativo || b->estado != BOSS_ATTACK)
        return;
    if (jogador->estado == SAM_DEAD)
        return;

    double agora = al_get_time();

    float dist = dist_bordas(b, jogador);
    if (!hitboxes_sobrepoem(b, jogador) && dist > BOSS_ALCANCE_ATAQUE)
        return;

    if ((agora - jogador->ultimo_dano) < BOSS_DELAY_DANO_JOGADOR)
        return;

    jogador->ultimo_dano = agora;

    if (vidas) perder_vida(vidas);

    float kb;
    if (b->direcao == 0)
        kb =  12.0f;
    else
        kb = -12.0f;

    jogador->mov.x += kb;

    if (jogador->estado != SAM_HURT && jogador->estado != SAM_DEAD)
    {
        jogador->estado      = SAM_HURT;
        jogador->hurt_inicio = agora;
    }

    if (sons) tocar(sons->boss_dano_sofrido);
}

/* ================================================================== */
/*  boss_desenhar                                                       */
/* ================================================================== */
void boss_desenhar(const Boss *b, const BossSprites *spr)
{
    if (!b->ativo || b->estado == BOSS_MORTO)
        return;

    ALLEGRO_BITMAP *sheet = NULL;
    int n_frames = 1;

    switch (b->estado)
    {
    case BOSS_IDLE:
        sheet = spr->idle;   n_frames = BOSS_FRAMES_IDLE;   break;
    case BOSS_WALK:
        sheet = spr->walk;   n_frames = BOSS_FRAMES_WALK;   break;
    case BOSS_ATTACK:
        sheet = spr->attack; n_frames = BOSS_FRAMES_ATTACK; break;
    case BOSS_DAMAGE:
        sheet = spr->damage; n_frames = BOSS_FRAMES_DAMAGE; break;
    case BOSS_DEATH:
        sheet = spr->death;  n_frames = BOSS_FRAMES_DEATH;  break;
    default: return;
    }

    if (!sheet)
        return;

    int frame_idx = (int)b->frame;
    if (frame_idx < 0)         frame_idx = 0;
    if (frame_idx >= n_frames) frame_idx = n_frames - 1;

    float sx = (float)(frame_idx * BOSS_SPRITE_W);

#if BOSS_SPRITE_OLHA_DIREITA
    int flags = (b->direcao == 1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
#else
    int flags = (b->direcao == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
#endif

    al_draw_scaled_bitmap(sheet,
                          sx, 0.0f,
                          BOSS_SPRITE_W, BOSS_SPRITE_H,
                          b->x, b->y,
                          BOSS_SPRITE_W * BOSS_ESCALA,
                          BOSS_SPRITE_H * BOSS_ESCALA,
                          flags);

#ifdef DEBUG_HITBOX
    {
        float hx1 = b->x + boss_hbx_offset_x(b);
        al_draw_rectangle(hx1,
                          b->y + BOSS_HBX_OFFSET_Y,
                          hx1 + BOSS_HBX_W,
                          b->y + BOSS_HBX_OFFSET_Y + BOSS_HBX_H,
                          al_map_rgb(255, 0, 0), 2);
    }
#endif
}

/* ================================================================== */
/*  boss_desenhar_hud                                                   */
/* ================================================================== */
void boss_desenhar_hud(const Boss *b, ALLEGRO_FONT *fonte_hud)
{
    if (!b->ativo || b->estado == BOSS_MORTO)
        return;

    float barra_w = 500.0f;
    float barra_h = 22.0f;
    float barra_x = LARGURA / 2.0f - barra_w / 2.0f;
    float barra_y = 80.0f;
    float preench = barra_w * ((float)b->hp / (float)b->hp_max);

    al_draw_filled_rectangle(barra_x - 2, barra_y - 2,
                             barra_x + barra_w + 2, barra_y + barra_h + 2,
                             al_map_rgba(0, 0, 0, 200));

    ALLEGRO_COLOR cor_hp;
    if (b->fase == 3)
        cor_hp = al_map_rgb(220, 50,  50);
    else if (b->fase == 2)
        cor_hp = al_map_rgb(220, 130, 20);
    else
        cor_hp = al_map_rgb(80,  200, 80);

    al_draw_filled_rectangle(barra_x, barra_y,
                             barra_x + preench, barra_y + barra_h,
                             cor_hp);

    al_draw_rectangle(barra_x, barra_y,
                      barra_x + barra_w, barra_y + barra_h,
                      al_map_rgb(200, 200, 200), 2);

    if (fonte_hud)
        al_draw_text(fonte_hud, al_map_rgb(255, 220, 50),
                     LARGURA / 2.0f, barra_y + barra_h + 4,
                     ALLEGRO_ALIGN_CENTER, "DEVORADOR DE MUNDOS");
}

/* ================================================================== */
/*  boss_derrotado                                                      */
/* ================================================================== */
int boss_derrotado(const Boss *b)
{
    return (b->estado == BOSS_MORTO && !b->morte_animando);
}