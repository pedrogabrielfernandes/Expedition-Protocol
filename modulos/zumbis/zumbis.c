#include "zumbis.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../tipos.h"
#include "../colisao/colisao.h"
#include "../sons/sons.h"
#include "../sanidade/sanidade.h"
#include "../vida/vida.h"

ProjetilAcido g_projeteis_acido[MAX_PROJETEIS_ACIDO];
ExplosaoAcida g_explosoes_acidas[MAX_EXPLOSOES_ACIDAS];
unsigned char g_explosao_acida_hit[MAX_EXPLOSOES_ACIDAS][MAX_ZUMBIS_TELA];

/* ================================================================== */
/*  Funções internas (static)                                          */
/* ================================================================== */
static int zumbis_vivos_na_tela(Horda *h)
{
    int c = 0;
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
        if (h->pool[i].vivo)
            c++;
    return c;
}

static int slot_livre(Horda *h)
{
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
        if (!h->pool[i].vivo)
            return i;
    return -1;
}

static void spawnar_zumbi(Horda *h, Sons *sons, int s,
                          float sx_, float sy_, int spawn_tipo)
{
    Inimigo *z = &h->pool[s];
    z->x = sx_;
    z->y = sy_;
    z->vel_y = 0.0f;
    z->x_inicial = sx_;
    z->y_inicial = sy_;
    z->patrol_base = sx_;
    if (rand() % 2 == 0){
        z->patrol_dir = 1.0f;}
    else{
        z->patrol_dir = -1.0f;}
    z->direcao = ALLEGRO_FLIP_HORIZONTAL;
    z->vivo = 1;
    z->estado = ZUM_WALK;
    z->frame = (float)(rand() % 8);
    z->tempo_hurt = 0.0;
    z->atacando_jogador = 0;
    z->tempo_ataque = 0.0;
    z->spawn_tipo = spawn_tipo;
    z->dano_aplicado = 0;
    z->atingido_no_ataque = 0;
    z->ataque_som_tocado = 0;
    z->ataque_resultado = 0;
    z->ultimo_dano_recebido = 0.0;
    z->tempo_stun = 0.0;
    z->stunado = 0;
    z->ja_stunado_no_dash = 0;
    z->tempo_morte = 0.0;
    z->explodiu = 0;
    z->mutante = 0;

    int roll = rand() % 100;

    if (roll < 15)
    {
        int acidos_vivos = 0;
        for (int ia = 0; ia < MAX_ZUMBIS_TELA; ia++)
            if (h->pool[ia].vivo && h->pool[ia].tipo == 2)
                acidos_vivos++;

        if (acidos_vivos >= 2)
        {
            z->tipo = 0;
            z->velocidade = 0.6f + (float)(rand() % 60) / 100.0f;
            z->vida = 5;
        }
        else
        {
            z->tipo = 2;
            z->velocidade = 0.8f + (float)(rand() % 30) / 100.0f;
            z->vida = 5;
        }
    }
    else if (roll < 40)
    {
        z->tipo = 1;
        z->velocidade = 1.8f + (float)(rand() % 80) / 100.0f;
        z->vida = 5;
        tocar(sons->som_velocista);
    }
    else
    {
        z->tipo = 0;
        z->velocidade = 0.6f + (float)(rand() % 60) / 100.0f;
        z->vida = 5;
    }
}

/* ================================================================== */
/*  HORDA  INIT                                                        */
/* ================================================================== */
void horda_init(Horda *h)
{
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
        h->pool[i].vivo = 0;
    h->total_spawned = 0;
    h->total_mortos = 0;
    h->timer_onda = 180;
    h->fase_concluida = 0;
    h->top_spawned = 0;
    h->top_ativo = 0;
    h->spawns_esq = 0;
    h->spawns_dir = 0;
}

/* ================================================================== */
/*  HORDA  SPAWN                                                       */
/* ================================================================== */
void horda_atualizar_spawn(Horda *h, Sons *sons, float jogador_y)
{
    if (h->fase_concluida || h->total_spawned >= TOTAL_ZUMBIS_FASE)
        return;
    if (jogador_y < SPAWN_MIN_JOGADOR_Y)
        return;

    int acido_vivo = 0;
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
        if (h->pool[i].vivo && h->pool[i].tipo == 2)
            acido_vivo = 1;
    if (acido_vivo)
        return;

    if (zumbis_vivos_na_tela(h) > 2)
        return;
    if (--h->timer_onda <= 0)
    {
        int restam = TOTAL_ZUMBIS_FASE - h->total_spawned;
        int para_spawn;
    if (restam < 5){    
        para_spawn = restam;}
        else{
            para_spawn = 5;}
        tocar(sons->horda_som);
        for (int n = 0; n < para_spawn; n++)
        {
            int s = slot_livre(h);
            if (s < 0)
                break;
            float sx_, sy_;
            int lado = rand() % 2;
            if (h->spawns_esq >= 25)
                lado = 1;
            if (h->spawns_dir >= 25)
                lado = 0;
            if (lado == 0)
            {
                sx_ = SPAWN_TOP_LEFT_X + (float)(rand() % 180);
                sy_ = SPAWN_TOP_LEFT_Y;
                h->spawns_esq++;
            }
            else
            {
                sx_ = SPAWN_DIR_X_MIN + (float)(rand() % (SPAWN_DIR_X_MAX - SPAWN_DIR_X_MIN));
                sy_ = SPAWN_DIR_Y;
                h->spawns_dir++;
            }
            spawnar_zumbi(h, sons, s, sx_, sy_, 0);
            h->total_spawned++;
        }
        h->timer_onda = 120;
    }
}

/* ================================================================== */
/*  HORDA  FÍSICA                                                      */
/* ================================================================== */
void horda_atualizar_fisica(Horda *h, ALLEGRO_BITMAP *mapa)
{
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
    {
        Inimigo *z = &h->pool[i];
        if (!z->vivo || z->estado == ZUM_DEAD)
            continue;
        z->vel_y += GRAVIDADE_ZUMBI;
        if (z->vel_y > MAX_QUEDA_ZUMBI)
            z->vel_y = MAX_QUEDA_ZUMBI;
        float ny = z->y + z->vel_y;
        if (z->vel_y > 0.0f)
        {
            if (zumbi_colide_vertical(mapa, z->x, ny, z->vel_y))
            {
                while (!zumbi_colide_vertical(mapa, z->x, z->y + 1.0f, 1.0f))
                    z->y += 1.0f;
                z->vel_y = 0.0f;
            }
            else
                z->y = ny;
        }
        else if (z->vel_y < 0.0f)
        {
            if (zumbi_colide_vertical(mapa, z->x, ny, z->vel_y))
                z->vel_y = 0.0f;
            else
                z->y = ny;
        }
        if (z->y < 0.0f)
            z->y = 0.0f;
        if (z->y > ALTURA + 300)
            z->vivo = 0;
    }
}

/* ================================================================== */
/*  HORDA  MOVIMENTO / IA                                              */
/* ================================================================== */
void horda_atualizar_movimento(Horda *h, Jogador *j, Sons *sons,
                               ALLEGRO_BITMAP *mapa)
{
    double agora = al_get_time();
    (void)sons;

    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
    {
        Inimigo *z = &h->pool[i];
        if (!z->vivo)
            continue;

        if (z->estado == ZUM_DEAD)
        {
            if (z->tipo == 2)
            {
                if (!z->explodiu)
                {
                    if (z->frame < FRAMES_ACIDO_DEAD - 1)
                        z->frame += 0.08f;
                    else
                        z->frame = FRAMES_ACIDO_DEAD - 1;

                    double elapsed = agora - z->tempo_morte;
                    if (elapsed >= 2.25)
                    {
                        float ex = z->x + ACIDO_HBX_OFFSET_X + ACIDO_HBX_W / 2.0f;
                        float ey = z->y + ACIDO_HBX_OFFSET_Y + ACIDO_HBX_H / 2.0f;
                        explosao_acida_spawn(ex, ey);
                        tocar(sons->explosao_acida);
                        tocar(sons->explosao_acida2);
                        z->explodiu = 1;
                    }
                }
                else
                {
                    z->vivo = 0;
                }
            }
            else
            {
                z->frame += 0.08f;
                if (z->frame >= FRAMES_ZUMBI_DEAD)
                    z->vivo = 0;
            }
            continue;
        }

        /* STUN */
        if (z->stunado)
        {
            double elapsed = agora - z->tempo_stun;
            if (elapsed < 1.5)
            {
                z->x += sinf((float)elapsed * 40.0f) * 0.8f;
                z->estado = ZUM_IDLE;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ZUMBI_IDLE)
                    z->frame = 0.0f;
                continue;
            }
            else
            {
                z->stunado = 0;
                z->atacando_jogador = 0;
                z->dano_aplicado = 0;
                z->frame = 0;
                z->estado = ZUM_WALK;
            }
        }

        if (z->estado == ZUM_HURT)
        {
            double elapsed = agora - z->tempo_hurt;
            z->frame = (float)(elapsed / KNOCKBACK_ZUMBI_DURACAO) * FRAMES_ZUMBI_HURT;
            if (elapsed >= KNOCKBACK_ZUMBI_DURACAO)
            {
                z->frame = 0;
                z->estado = ZUM_WALK;
                z->atacando_jogador = 0;
                z->dano_aplicado = 0;
            }
            continue;
        }

        if (z->estado == ZUM_ATTACK || z->estado == ZUM_BITE)
        {
            if (z->tipo == 2)
            {
                int frame_antes = (int)z->frame;
                z->frame += 0.045f;
                int frame_depois = (int)z->frame;

                if (frame_antes < ZUMBI_ACIDO_FRAME_DISPARO &&
                    frame_depois >= ZUMBI_ACIDO_FRAME_DISPARO &&
                    !z->ataque_som_tocado)
                {
                    float bx_ = z->x + ACIDO_HBX_OFFSET_X + ACIDO_HBX_W / 2.0f;
                    float by_ = z->y + ACIDO_HBX_OFFSET_Y + ACIDO_HBX_H / 2.0f;
                    projetil_acido_spawn(bx_, by_, z->direcao);
                    tocar(sons->cuspindo_acido);
                    z->ataque_som_tocado = 1;
                }

                if (z->frame >= FRAMES_ACIDO_ATTACK)
                {
                    z->frame = 0;
                    z->estado = ZUM_IDLE;
                    z->atacando_jogador = 0;
                    z->dano_aplicado = 0;
                    z->ataque_som_tocado = 0;
                    z->ataque_resultado = 0;
                }
                continue;
            }

            int max_f;
                if (z->estado == ZUM_ATTACK)
                    max_f = FRAMES_ZUMBI_ATTACK1;
                else
                    max_f = FRAMES_ZUMBI_BITE;
            float vel_an;
                if (z->tipo == 1)
                    vel_an = 0.15f;
                else
                    vel_an = 0.10f;
            z->frame += vel_an;
            if (z->frame >= max_f)
            {
                z->frame = 0;
                z->estado = ZUM_WALK;
                z->atacando_jogador = 0;
                z->dano_aplicado = 0;
                z->ataque_som_tocado = 0;
                z->ataque_resultado = 0;
            }
            continue;
        }

        float jx = j->mov.x, jy = j->mov.y;
        float zumbi_cx = z->x + ZUMBI_HBX_OFFSET_X + ZUMBI_HBX_W / 2.0f;
        float zumbi_cy = z->y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H / 2.0f;
        float jog_cx = jx + HITBOX_W / 2.0f;
        float jog_cy = jy + HITBOX_H / 2.0f;

        int jogador_morto = (j->estado == SAM_DEAD);
        float jy_ref;
            if (j->no_chao)
                jy_ref = jy;
            else
                jy_ref = j->y_chao;
        int zumbi_abaixo = (z->y > jy_ref + ZUMBI_ABAIXO_MARGEM);
        int jogador_alto = (jy_ref < NIVEL_ALTO_Y);

        if ((!jogador_morto && zumbi_abaixo) ||
            (!jogador_morto && jogador_alto &&
             !(fabsf(jog_cx - zumbi_cx) < 250.0f &&
               fabsf(jog_cy - zumbi_cy) < 250.0f)))
        {
            if (agora - z->tempo_ataque > 1.5)
            {
                z->tempo_ataque = agora;
                int r = rand() % 3;
                if (r == 0)
            z->patrol_dir = -1.0f;
                else if (r == 1)
                    z->patrol_dir = 1.0f;
                else
                    z->patrol_dir = 0.0f;
            }
            if (z->patrol_dir != 0.0f)
            {
                float nx_z = z->x + PATRULHA_VEL * z->patrol_dir;
                if (nx_z < PATROL_AREA_X_MIN || nx_z > PATROL_AREA_X_MAX)
                    z->patrol_dir = -z->patrol_dir;
                else if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                else
                z->patrol_dir = -z->patrol_dir;
                if (z->patrol_dir > 0)
                z->direcao = 0;
                else
                {
                    z->direcao = ALLEGRO_FLIP_HORIZONTAL;
                }
                z->estado = ZUM_WALK;
                z->frame += 0.05f;
                if (z->frame >= FRAMES_ZUMBI_WALK)
                    z->frame = 0.0f;
            }
            else
            {
                z->estado = ZUM_IDLE;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ZUMBI_IDLE)
                    z->frame = 0.0f;
            }
            continue;
        }

        if (z->tipo == 2)
        {
            float dx_signed = jog_cx - zumbi_cx;
            float dist = fabsf(dx_signed);

            if (dist < ZUMBI_ACIDO_DIST_MIN)
            {
                float passo = z->velocidade;
                float nx_z;
                    if (dx_signed > 0)
                        nx_z = z->x - passo;
                    else
                        nx_z = z->x + passo;
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                if (dx_signed > 0){
                    z->direcao = ALLEGRO_FLIP_HORIZONTAL;}
                else{
                    z->direcao = 0;}
                z->estado = ZUM_WALK;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ACIDO_WALK)
                    z->frame = 0.0f;
            }
            else if (dist > ZUMBI_ACIDO_DIST_MAX)
            {
                float passo = z->velocidade * 0.5f;
                float nx_z;
                    if (dx_signed > 0)
                        nx_z = z->x + passo;
                    else
                        nx_z = z->x - passo;
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                if (dx_signed > 0)
                    z->direcao = 0;
                else
                    z->direcao = ALLEGRO_FLIP_HORIZONTAL;
                z->estado = ZUM_WALK;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ACIDO_WALK)
                    z->frame = 0.0f;
            }
            else
            {
                if (dx_signed > 0)
                    z->direcao = 0;
                else
                    z->direcao = ALLEGRO_FLIP_HORIZONTAL;
                if (agora - z->tempo_ataque > ZUMBI_ACIDO_COOLDOWN_ATAQUE)
                {
                    z->tempo_ataque = agora;
                    z->estado = ZUM_ATTACK;
                    z->frame = 0;
                    z->ataque_som_tocado = 0;
                }
                else
                {
                    z->estado = ZUM_IDLE;
                    z->frame += 0.03f;
                    if (z->frame >= FRAMES_ACIDO_IDLE)
                        z->frame = 0.0f;
                }
            }
            continue;
        }

        float dx = fabsf(jog_cx - zumbi_cx);
        float dy = fabsf(jog_cy - zumbi_cy);

        if (dx < 55.0f && dy < 60.0f)
        {
            if (!z->atacando_jogador || (agora - z->tempo_ataque > 1.4))
            {
                z->atacando_jogador = 1;
                z->tempo_ataque = agora;
                z->frame = 0;
                z->dano_aplicado = 0;
                z->ataque_som_tocado = 0;
                z->ataque_resultado = 0;
                z->x += ((i % 5) - 2) * 6.0f;
                if (z->tipo == 1)
                    z->estado = ZUM_BITE;
                else
                    z->estado = ZUM_ATTACK;
            }
        }
        else
        {
            float nx_z;
            if (jog_cx < zumbi_cx)
            {
                nx_z = z->x - z->velocidade;
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                z->direcao = ALLEGRO_FLIP_HORIZONTAL;
            }
            else
            {
                nx_z = z->x + z->velocidade;
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                z->direcao = 0;
            }
            if (z->tipo == 1)
                z->estado = ZUM_RUN;
            else
                z->estado = ZUM_WALK;
           float va;
            if (z->tipo == 1)
                va = 0.14f;
            else
                va = 0.07f;

            int mf;
            if (z->tipo == 1)
                mf = FRAMES_ZUMBI_RUN;
            else
                mf = FRAMES_ZUMBI_WALK;
                        if (z->frame >= mf)
                            z->frame = 0.0f;
                    }
                }
            }

/* ================================================================== */
/*  HORDA  VERIFICAR ATAQUE DO SAMURAI                                 */
/* ================================================================== */
void horda_verificar_ataque(Horda *h, Jogador *jog, Sanidade *san, Sons *sons)
{
    if (!jog->atacando)
        return;
    int frame_atual = (int)jog->frame_ataque;
    if (frame_atual < SAM_ATK_FRAME_INICIO || frame_atual >= SAM_ATK_FRAME_FIM)
    {
        jog->acertos_no_swing = 0;
        return;
    }
    if (jog->acertos_no_swing >= 2)
        return;

    float atk_h = SAM_ATK_H;
    float atk_w = SAM_ATK_W + 20.0f;
    float atk_y = jog->mov.y + (HITBOX_H - atk_h) / 2.0f;
    float jog_cx = jog->mov.x + HITBOX_W / 2.0f;
    float offset = 10.0f;
    float atk_x;
    if (jog->direcao == 0)
        atk_x = jog_cx + offset;
    else
        atk_x = jog_cx - atk_w - offset;

    for (int ip = 0; ip < MAX_PROJETEIS_ACIDO; ip++)
    {
        ProjetilAcido *p = &g_projeteis_acido[ip];
        if (!p->ativo || p->impactou)
            continue;
        float px2 = p->x - ACIDO_PROJETIL_HBX_W / 2.0f;
        float py2 = p->y - ACIDO_PROJETIL_HBX_H / 2.0f;
        if (atk_x < px2 + ACIDO_PROJETIL_HBX_W && atk_x + atk_w > px2 &&
            atk_y < py2 + ACIDO_PROJETIL_HBX_H && atk_y + atk_h > py2)
        {
            p->impactou = 1;
            p->frame = 5.0f;
            tocar(sons->impacto_espada);
        }
    }

    double agora = al_get_time();
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
    {
        if (jog->acertos_no_swing >= 2)
            break;
        Inimigo *z = &h->pool[i];
        if (!z->vivo || z->estado == ZUM_DEAD || z->atingido_no_ataque)
            continue;
        if (z->estado == ZUM_HURT)
            continue;
        if (agora - z->ultimo_dano_recebido < KNOCKBACK_ZUMBI_DURACAO)
            continue;

        float hzx, hzy, hzw, hzh;
        if (z->tipo == 2)
        {
            hzx = z->x + ACIDO_HBX_OFFSET_X;
            hzy = z->y + ACIDO_HBX_OFFSET_Y;
            hzw = ACIDO_HBX_W;
            hzh = ACIDO_HBX_H;
        }
        else
        {
            hzx = z->x + ZUMBI_HBX_OFFSET_X;
            hzy = z->y + ZUMBI_HBX_OFFSET_Y;
            hzw = ZUMBI_HBX_W;
            hzh = ZUMBI_HBX_H;
        }

        if (atk_x < hzx + hzw && atk_x + atk_w > hzx &&
            atk_y < hzy + hzh && atk_y + atk_h > hzy)
        {
            tocar(sons->acerto_zumbi);
            int dano;
            if (jog->tipo_ataque == 3)
                dano = 2;
            else if (jog->tipo_ataque == 2)
                dano = jog->dano_atk2_atual;
            else
                dano = 1;
            z->vida -= dano;
            if (z->vida < 0)
                z->vida = 0;
            z->atingido_no_ataque = 1;
            z->ultimo_dano_recebido = agora;
            jog->acertos_no_swing++;

            float kb_dir;
            if (jog->direcao == 0)
                kb_dir = 1.0f;
            else
                kb_dir = -1.0f;
            z->x += kb_dir * KNOCKBACK_ZUMBI_X;

            if (z->vida <= 0)
            {
                z->estado = ZUM_DEAD;
                z->frame = 0;
                if (z->tipo == 2)
                {
                    z->tempo_morte = al_get_time();
                    z->explodiu = 0;
                    tocar(sons->morte_acido);
                }
                else if (z->tipo == 1)
                    tocar(sons->morte_velocista);
                else
                    tocar(sons->morte_zumbi);
                h->total_mortos++;
                san->zumbis_mortos++;
                san->ultimo_abate = al_get_time();
                san->regenerando = 0;
                atualizar_sanidade(san);
                if (h->total_mortos >= TOTAL_ZUMBIS_FASE)
                    h->fase_concluida = 1;
            }
            else
            {
                if (z->tipo == 1)
                    tocar(sons->dano_sofrido_velocista);
                else if (z->tipo == 2)
                    tocar(sons->dano_zumbi_acido);
                else
                    tocar(sons->dano_zumbi);
                if (z->estado != ZUM_HURT)
                {
                    z->estado = ZUM_HURT;
                    z->frame = 0;
                    z->tempo_hurt = al_get_time();
                }
            }
        }
    }
}

/* ================================================================== */
/*  HORDA  DANO AO JOGADOR                                             */
/* ================================================================== */
void horda_verificar_dano_jogador(Horda *h, Jogador *jog,
                                  VidaStatus *vidas, Sons *sons)
{
    double agora = al_get_time();
    if (agora - jog->ultimo_dano <= TEMPO_INVUL)
        return;

    const float alcance_golpe = 55.0f;

    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
    {
        Inimigo *z = &h->pool[i];
        if (!z->vivo || z->estado == ZUM_DEAD)
            continue;
        if (z->tipo == 2)
            continue;
        if (z->estado != ZUM_ATTACK && z->estado != ZUM_BITE)
            continue;
        if (z->dano_aplicado)
            continue;

        int frame_ativo = (int)z->frame;
        int inicio_dano;
        if (z->estado == ZUM_BITE)
            inicio_dano = 4;
        else
            inicio_dano = 2;
        if (frame_ativo < inicio_dano)
            continue;

        float body_x = z->x + ZUMBI_HBX_OFFSET_X;
        float body_y = z->y + ZUMBI_HBX_OFFSET_Y;
        float body_centro = body_x + ZUMBI_HBX_W / 2.0f;
        float jog_centro = jog->mov.x + HITBOX_W / 2.0f;
        float atk_w = alcance_golpe + ZUM_ATK_REACH;
        float atk_x;
        if (body_centro < jog_centro)
            atk_x = body_x + ZUMBI_HBX_W;
        else
            atk_x = body_x - atk_w;
        float atk_y = body_y;

        float jx = jog->mov.x, jy = jog->mov.y,
              jw = HITBOX_W, jh = HITBOX_H;
        int acertou = (atk_x < jx + jw && atk_x + atk_w > jx &&
                       atk_y < jy + jh && atk_y + ZUM_ATK_H > jy);

        if (acertou)
        {
            perder_vida(vidas);
            jog->ultimo_dano = agora;
            tocar(sons->dano_sofrido);
            tocar(sons->zumbi_knockback);
            if (z->tipo == 1)
                tocar(sons->dano_velocista);
            else
                tocar(sons->dano);

            float kb_dir;
            if (body_centro < jog_centro)
                kb_dir = 1.0f;
            else
                kb_dir = -1.0f;

            if (body_centro < jog_centro)
                jog->direcao = ALLEGRO_FLIP_HORIZONTAL;
            else
                jog->direcao = 0;            
            jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X;
            jog->mov.vel_y = -6.0f;
            z->dano_aplicado = 1;

            if (jog->estado != SAM_DEAD)
            {
                jog->estado = SAM_HURT;
                jog->frame_hurt = 0;
                jog->hurt_inicio = agora;
            }
        }
        else
        {
            if (!z->ataque_som_tocado && frame_ativo == inicio_dano)
            {
                z->ataque_som_tocado = 1;
                if (z->tipo == 1)
                    tocar(sons->ataque_velocista);
                else
                    tocar(sons->dano_miss);
            }
            z->dano_aplicado = 1;
        }
    }
}

/* ================================================================== */
/*  HORDA  DESENHO                                                     */
/* ================================================================== */
void horda_desenhar(Horda *h, ZumbiSprites *spr,
                    ZumbiAcidoSprites *spr_acido)
{
    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
    {
        Inimigo *z = &h->pool[i];
        if (!z->vivo)
            continue;

        ALLEGRO_BITMAP *sheet = NULL;
        int max_f = FRAMES_ZUMBI_WALK;

        if (z->tipo == 2)
        {
            switch (z->estado)
            {
            case ZUM_WALK:
                sheet = spr_acido->walk;
                max_f = FRAMES_ACIDO_WALK;
                break;
            case ZUM_ATTACK:
                sheet = spr_acido->attack;
                max_f = FRAMES_ACIDO_ATTACK;
                break;
            case ZUM_HURT:
                sheet = spr_acido->hurt;
                max_f = FRAMES_ACIDO_HURT;
                break;
            case ZUM_DEAD:
                sheet = spr_acido->dead;
                max_f = FRAMES_ACIDO_DEAD;
                break;
            default:
                sheet = spr_acido->idle;
                max_f = FRAMES_ACIDO_IDLE;
                break;
            }
        }
        else
        {
            switch (z->estado)
            {
            case ZUM_WALK:
                sheet = spr->walk;
                max_f = FRAMES_ZUMBI_WALK;
                break;
            case ZUM_RUN:
                sheet = spr->run;
                max_f = FRAMES_ZUMBI_RUN;
                break;
            case ZUM_ATTACK:
                sheet = spr->attack1;
                max_f = FRAMES_ZUMBI_ATTACK1;
                break;
            case ZUM_BITE:
                sheet = spr->bite;
                max_f = FRAMES_ZUMBI_BITE;
                break;
            case ZUM_HURT:
                sheet = spr->hurt;
                max_f = FRAMES_ZUMBI_HURT;
                break;
            case ZUM_DEAD:
                sheet = spr->dead;
                max_f = FRAMES_ZUMBI_DEAD;
                break;
            default:
                sheet = spr->idle;
                max_f = FRAMES_ZUMBI_IDLE;
                break;
            }
        }
        if (!sheet)
           if (z->tipo == 2)
                sheet = spr_acido->walk;
            else
                sheet = spr->walk;

        int fz = (int)z->frame % max_f;

        if (z->tipo == 2)
        {
            ALLEGRO_COLOR tint_acido = al_map_rgb(255, 255, 255);
            if (z->estado == ZUM_DEAD && !z->explodiu && z->tempo_morte > 0.0)
            {
                double elapsed = al_get_time() - z->tempo_morte;
                float progresso = (float)(elapsed / 2.0);
                if (progresso > 1.0f)
                    progresso = 1.0f;
                float pulso = 0.5f + 0.5f * sinf((float)al_get_time() * 12.0f);
                unsigned char g = (unsigned char)(255.0f * (0.5f + 0.5f * progresso * pulso));
                unsigned char rb = (unsigned char)(255.0f * (1.0f - progresso * 0.8f));
                tint_acido = al_map_rgb(rb, g, rb);
            }
            else if (z->mutante)
            {
                tint_acido = al_map_rgb(120, 255, 120);
            }
            al_draw_tinted_scaled_bitmap(sheet, tint_acido,
                                         fz * ZUMBI_ACIDO_SRC_W, 0,
                                         ZUMBI_ACIDO_SRC_W, ZUMBI_ACIDO_SRC_H,
                                         z->x, z->y - 40,
                                         ZUMBI_ACIDO_DRAW_W, ZUMBI_ACIDO_DRAW_H,
                                         z->direcao);
        }
        else
        {
           ALLEGRO_COLOR tint;
            if (z->tipo == 1)
                tint = al_map_rgb(255, 120, 120);
            else
                tint = al_map_rgb(255, 255, 255);
            al_draw_tinted_scaled_bitmap(sheet, tint,
                                         fz * ZUMBI_SRC_W, 0,
                                         ZUMBI_SRC_W, ZUMBI_SRC_H,
                                         z->x, z->y,
                                         ZUMBI_DRAW_W, ZUMBI_DRAW_H,
                                         z->direcao);
        }

        if (z->estado != ZUM_DEAD)
        {
            float bw2 = 55.0f;
           float draw_w;
            if (z->tipo == 2)
                draw_w = ZUMBI_ACIDO_DRAW_W;
            else
                draw_w = ZUMBI_DRAW_W;

            float draw_y;
            if (z->tipo == 2)
                draw_y = z->y - 40;
            else
                draw_y = z->y;

            float offset_barra;
            if (z->tipo == 2)
                offset_barra = 60.0f;
            else
                offset_barra = 20.0f;
            float bx2 = z->x + draw_w / 2.0f - bw2 / 2.0f, by2 = draw_y + offset_barra;
            float vida_pct = (float)z->vida / 5.0f;
            al_draw_filled_rectangle(bx2, by2, bx2 + bw2, by2 + 6, al_map_rgb(80, 0, 0));
            al_draw_filled_rectangle(bx2, by2, bx2 + bw2 * vida_pct, by2 + 6, al_map_rgb(255, 0, 0));
        }
    }
}

/* ================================================================== */
/*  PROJÉTIL DE ÁCIDO                                                  */
/* ================================================================== */
void projetil_acido_spawn(float x, float y, int direcao)
{
    for (int i = 0; i < MAX_PROJETEIS_ACIDO; i++)
    {
        if (!g_projeteis_acido[i].ativo)
        {
            if (direcao == 0)
                g_projeteis_acido[i].x = x + 30.0f;
            else
                g_projeteis_acido[i].x = x - 30.0f;
            g_projeteis_acido[i].y = y - 35;
            g_projeteis_acido[i].direcao = direcao;
            g_projeteis_acido[i].frame = 0.0f;
            g_projeteis_acido[i].ativo = 1;
            g_projeteis_acido[i].dist_percorrida = 0.0f;
            g_projeteis_acido[i].impactou = 0;
            return;
        }
    }
}

void projeteis_acido_atualizar(ALLEGRO_BITMAP *mapa, Jogador *jog,
                               VidaStatus *vidas, Sons *sons)
{
    double agora = al_get_time();

    for (int i = 0; i < MAX_PROJETEIS_ACIDO; i++)
    {
        ProjetilAcido *p = &g_projeteis_acido[i];
        if (!p->ativo)
            continue;

        if (p->impactou)
        {
            p->frame += 0.22f;
            if (p->frame >= FRAMES_ACIDO_PROJETIL)
                p->ativo = 0;
            continue;
        }

        float vx;
        if (p->direcao == 0)
            vx = ACIDO_PROJETIL_VEL;
        else
            vx = -ACIDO_PROJETIL_VEL;
        p->x += vx;
        p->dist_percorrida += ACIDO_PROJETIL_VEL;

        p->frame += 0.22f;
        if (p->frame >= 5.0f)
            p->frame = 0.0f;

        if (pixel_solido(mapa, (int)p->x, (int)p->y))
        {
            p->impactou = 1;
            p->frame = 5.0f;
            tocar(sons->acido_impacto);
            continue;
        }

        if (p->x < -60.0f || p->x > LARGURA + 60.0f)
        {
            p->ativo = 0;
            continue;
        }

        if (p->dist_percorrida >= ACIDO_PROJETIL_DIST_MAX)
        {
            p->impactou = 1;
            p->frame = 5.0f;
            tocar(sons->acido_impacto);
            continue;
        }

        float px_ = p->x - ACIDO_PROJETIL_HBX_W / 2.0f;
        float py_ = p->y - ACIDO_PROJETIL_HBX_H / 2.0f;

        if (jog->estado != SAM_DEAD &&
            agora - jog->ultimo_dano > TEMPO_INVUL &&
            px_ < jog->mov.x + HITBOX_W && px_ + ACIDO_PROJETIL_HBX_W > jog->mov.x &&
            py_ < jog->mov.y + HITBOX_H && py_ + ACIDO_PROJETIL_HBX_H > jog->mov.y)
        {
            perder_vida(vidas);
            jog->ultimo_dano = agora;
            tocar(sons->dano_sofrido);
            tocar(sons->acido_impacto);

           float kb_dir;
            if (p->direcao == 0)
                kb_dir = 1.0f;
            else
                kb_dir = -1.0f;

            jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X * 0.5f;  
            if (p->direcao == 0)
                jog->direcao = ALLEGRO_FLIP_HORIZONTAL;
            else
                jog->direcao = 0;

            if (jog->estado != SAM_DEAD)
            {
                jog->estado = SAM_HURT;
                jog->frame_hurt = 0;
                jog->hurt_inicio = agora;
            }

            p->impactou = 1;
            p->frame = 5.0f;
        }
    }
}

void projeteis_acido_desenhar(ALLEGRO_BITMAP *spr)
{
    if (!spr)
        return;
    for (int i = 0; i < MAX_PROJETEIS_ACIDO; i++)
    {
        ProjetilAcido *p = &g_projeteis_acido[i];
        if (!p->ativo)
            continue;
        int f = (int)p->frame % FRAMES_ACIDO_PROJETIL;
        al_draw_scaled_bitmap(spr,
                              f * ACIDO_PROJETIL_SRC_W, 0,
                              ACIDO_PROJETIL_SRC_W, ACIDO_PROJETIL_SRC_H,
                              p->x - ACIDO_PROJETIL_DRAW_W / 2.0f,
                              p->y - ACIDO_PROJETIL_DRAW_H / 2.0f,
                              ACIDO_PROJETIL_DRAW_W, ACIDO_PROJETIL_DRAW_H,
                              p->direcao);
    }
}

/* ================================================================== */
/*  EXPLOSÃO DE ÁCIDO                                                  */
/* ================================================================== */
void explosao_acida_spawn(float x, float y)
{
    for (int i = 0; i < MAX_EXPLOSOES_ACIDAS; i++)
    {
        if (!g_explosoes_acidas[i].ativo)
        {
            g_explosoes_acidas[i].x = x;
            g_explosoes_acidas[i].y = y;
            g_explosoes_acidas[i].frame = 0.0f;
            g_explosoes_acidas[i].ativo = 1;
            memset(g_explosao_acida_hit[i], 0, sizeof(g_explosao_acida_hit[i]));
            return;
        }
    }
}

void explosoes_acidas_atualizar(Horda *h, Jogador *jog, VidaStatus *vidas,
                                Sanidade *san, Sons *sons)
{
    double agora = al_get_time();

    for (int i = 0; i < MAX_EXPLOSOES_ACIDAS; i++)
    {
        ExplosaoAcida *e = &g_explosoes_acidas[i];
        if (!e->ativo)
            continue;

        if (e->frame >= 5.0f &&
            jog->estado != SAM_DEAD &&
            agora - jog->ultimo_dano > TEMPO_INVUL)
        {
            float dx = (jog->mov.x + HITBOX_W / 2.0f) - e->x;
            float dy = (jog->mov.y + HITBOX_H / 2.0f) - e->y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < EXPLOSAO_ACIDA_RAIO)
            {
                for (int v = 0; v < MAX_VIDAS; v++)
                    vidas[v].ativa = 0;

                jog->ultimo_dano = agora;
                tocar(sons->dano_sofrido);

              float kb_dir;
                if (dx >= 0)
                    kb_dir = 1.0f;
                else
                    kb_dir = -1.0f;

                jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X;  // ← depois
                jog->mov.vel_y = -6.0f;

                    if (kb_dir > 0)
                        jog->direcao = ALLEGRO_FLIP_HORIZONTAL;
                    else
                        jog->direcao = 0;

                if (jog->estado != SAM_DEAD)
                {
                    jog->estado = SAM_DEAD;
                    jog->frame_dead = 0;
                    jog->morte_animando = 1;
                    jog->som_morrendo_tocado = 0;
                }
            }
        }

        if (e->frame >= 5.0f)
        {
            for (int zi = 0; zi < MAX_ZUMBIS_TELA; zi++)
            {
                if (g_explosao_acida_hit[i][zi])
                    continue;

                Inimigo *z = &h->pool[zi];
                if (!z->vivo || z->estado == ZUM_DEAD)
                    continue;

                float zx, zy, zw, zh;
                if (z->tipo == 2)
                {
                    zx = z->x + ACIDO_HBX_OFFSET_X;
                    zy = z->y + ACIDO_HBX_OFFSET_Y;
                    zw = ACIDO_HBX_W;
                    zh = ACIDO_HBX_H;
                }
                else
                {
                    zx = z->x + ZUMBI_HBX_OFFSET_X;
                    zy = z->y + ZUMBI_HBX_OFFSET_Y;
                    zw = ZUMBI_HBX_W;
                    zh = ZUMBI_HBX_H;
                }

                float zcx = zx + zw / 2.0f;
                float zcy = zy + zh / 2.0f;
                float zdx = zcx - e->x;
                float zdy = zcy - e->y;
                float zdist = sqrtf(zdx * zdx + zdy * zdy);

                if (zdist < EXPLOSAO_ACIDA_RAIO)
                {
                    g_explosao_acida_hit[i][zi] = 1;

                    if (z->tipo == 2 && !z->mutante)
                    {
                        z->mutante = 1;
                        z->velocidade = 1.8f + (float)(rand() % 80) / 100.0f;
                        float kb_dir;
                            if (zdx >= 0)
                                kb_dir = 1.0f;
                            else
                                kb_dir = -1.0f;
                        z->x += kb_dir * KNOCKBACK_ZUMBI_X * 0.5f;
                        tocar(sons->dano_zumbi_acido);
                        continue;
                    }

                    z->vida = 0;
                    z->estado = ZUM_DEAD;
                    z->frame = 0;

                    float kb_dir;
                        if (zdx >= 0)
                            kb_dir = 1.0f;
                        else
                            kb_dir = -1.0f;
                    z->x += kb_dir * KNOCKBACK_ZUMBI_X;

                    if (z->tipo == 2)
                    {
                        z->tempo_morte = al_get_time();
                        z->explodiu = 0;
                        tocar(sons->morte_acido);
                    }
                    else if (z->tipo == 1)
                        tocar(sons->morte_velocista);
                    else
                        tocar(sons->morte_zumbi);

                    h->total_mortos++;
                    san->zumbis_mortos++;
                    san->ultimo_abate = al_get_time();
                    san->regenerando = 0;
                    atualizar_sanidade(san);
                    if (h->total_mortos >= TOTAL_ZUMBIS_FASE)
                        h->fase_concluida = 1;
                }
            }
        }

        if (e->frame >= 5.0f && e->frame < 8.0f)
            e->frame += 0.25f;
        else
            e->frame += 0.20f;

        if (e->frame >= FRAMES_EXPLOSAO_ACIDA)
            e->ativo = 0;
    }
}

void explosoes_acidas_desenhar(ALLEGRO_BITMAP *frames[])
{
    for (int i = 0; i < MAX_EXPLOSOES_ACIDAS; i++)
    {
        ExplosaoAcida *e = &g_explosoes_acidas[i];
        if (!e->ativo)
            continue;

        int f = (int)e->frame;
        if (f < 0)
            f = 0;
        if (f >= FRAMES_EXPLOSAO_ACIDA)
            f = FRAMES_EXPLOSAO_ACIDA - 1;
        if (!frames[f])
            continue;

        al_draw_scaled_bitmap(frames[f],
                              0, 0,
                              al_get_bitmap_width(frames[f]),
                              al_get_bitmap_height(frames[f]),
                              e->x - EXPLOSAO_ACIDA_DRAW_W / 2.0f,
                              e->y - EXPLOSAO_ACIDA_DRAW_H / 2.0f,
                              EXPLOSAO_ACIDA_DRAW_W,
                              EXPLOSAO_ACIDA_DRAW_H,
                              0);
    }
}