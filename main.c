/* ================================================================== */
/*  main.c    Samurai vs Zumbis                                       */
/* ================================================================== */
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "modulos/tipos.h"
#include "modulos/sons/sons.h"
#include "modulos/zumbis/zumbis.h"
#include "modulos/colisao/colisao.h"
#include "modulos/sanidade/sanidade.h"

/* ================================================================== */
/*  Globais de colisão                                                  */
/* ================================================================== */
unsigned char colisao[ALTURA][3000];
static ALLEGRO_BITMAP *jog_mapa_ptr = NULL;

/*  PROTÓTIPOS                                                          */
/* ================================================================== */
void perder_vida(VidaStatus *vidas);
int contar_vidas(VidaStatus *vidas);
void restaurar_vidas(VidaStatus *vidas, int quantidade);
void ordenar_ranking(float r[], int n);
int busca_binaria(float r[], int n, float v);
void carregar_ranking(Temporizador *t);
void salvar_ranking(Temporizador *t);
void atualizar_sanidade(Sanidade *san);
void gerar_mapa_colisao(ALLEGRO_BITMAP *mapa);
void pocao_tentar_spawn(Pocao *p, VidaStatus *vidas);
void pocao_atualizar(Pocao *p, Jogador *jog, VidaStatus *vidas, Sons *sons);
void pocao_desenhar(Pocao *p, ALLEGRO_BITMAP *spr);
bool pixel_solido(ALLEGRO_BITMAP *mapa, int x, int y);
bool colide_mapa(ALLEGRO_BITMAP *mapa, float x, float y);
void horda_atualizar_movimento(Horda *h, Jogador *j, Sons *sons, ALLEGRO_BITMAP *mapa);
void projetil_acido_spawn(float x, float y, int direcao);
void projeteis_acido_atualizar(ALLEGRO_BITMAP *mapa, Jogador *jog, VidaStatus *vidas, Sons *sons);
void projeteis_acido_desenhar(ALLEGRO_BITMAP *spr);
void explosao_acida_spawn(float x, float y);
void explosoes_acidas_atualizar(Horda *h, Jogador *jog, VidaStatus *vidas, Sanidade *san, Sons *sons);
void explosoes_acidas_desenhar(ALLEGRO_BITMAP *frames[]);

/* ================================================================== */
/*  PARTÍCULAS DECORATIVAS                                              */
/* ================================================================== */
Particula *criar_matriz_decorativa(int linhas, int colunas)
{
    int total = linhas * colunas;
    Particula *mat = (Particula *)malloc(total * sizeof(Particula));
    for (int i = 0; i < total; i++)
    {
        mat[i].x = (float)(rand() % LARGURA);
        mat[i].y = (float)(rand() % ALTURA);
        mat[i].y_origem = mat[i].y;
        mat[i].vy = 0.8f + (float)(rand() % 120) / 100.0f;
        mat[i].fase = (float)(rand() % 628) / 100.0f;
        mat[i].escala = 0.8f + (float)(rand() % 15) / 10.0f;
    }
    return mat;
}

void desenhar_matriz_fundo(Particula *mat, int total, double t)
{
    for (int i = 0; i < total; i++)
    {
        mat[i].y += mat[i].vy;
        mat[i].x += mat[i].vy * 0.5f;
        if (mat[i].y > (float)ALTURA + 4.0f || mat[i].x > (float)LARGURA + 4.0f)
        {
            mat[i].y = -4.0f;
            mat[i].x = (float)(rand() % LARGURA);
        }
        float pulso = 0.6f + 0.4f * sinf((float)t * 1.2f + mat[i].fase);
        unsigned char alpha = (unsigned char)(30.0f + 60.0f * pulso);
        float r = mat[i].escala;
        float x2 = mat[i].x + mat[i].vy * 0.8f;
        float y2 = mat[i].y + mat[i].vy * 1.6f;
        al_draw_line(mat[i].x, mat[i].y, x2, y2, al_map_rgba(200, 180, 120, alpha), r);
    }
}

void liberar_matriz(Particula *mat, int linhas)
{
    (void)linhas;
    free(mat);
}

/* ================================================================== */
/*  MENU PRINCIPAL                                                      */
/* ================================================================== */
OpcaoMenu executar_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                        ALLEGRO_BITMAP *bg_menu, ALLEGRO_FONT *fonte,
                        ALLEGRO_FONT *fonte_hud, Sons *sons)
{
    ALLEGRO_EVENT ev;
    OpcaoMenu opcao = MENU_JOGAR;
    OpcaoMenu opcao_ant = (OpcaoMenu)-1;

    while (1)
    {
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return MENU_SAIR;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            OpcaoMenu antes = opcao;
            if (ev.keyboard.keycode == ALLEGRO_KEY_A || ev.keyboard.keycode == ALLEGRO_KEY_LEFT)
                opcao = MENU_JOGAR;
            if (ev.keyboard.keycode == ALLEGRO_KEY_D || ev.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                opcao = MENU_SAIR;
            if (opcao != antes)
                tocar(sons->selecao_som);
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                if (opcao == MENU_JOGAR)
                    tocar(sons->jogar);
                else
                    tocar(sons->sair_som);
                al_rest(0.3);
                return opcao;
            }
        }

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            double t = al_get_time();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_scaled_bitmap(bg_menu, 0, 0,
                                  al_get_bitmap_width(bg_menu), al_get_bitmap_height(bg_menu),
                                  0, 0, LARGURA, ALTURA, 0);
            al_draw_filled_rectangle(0, 950, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 210));

            /* &#9472;&#9472; botão JOGAR &#9472;&#9472; */
            {
                int sel = (opcao == MENU_JOGAR);
                float cx = LARGURA / 2.4f, cy = 990.0f;
                if (sel)
                {
                    float pulso = 0.7f + 0.3f * sinf((float)t * 6.0f);
                    unsigned char ga = (unsigned char)(180.0f + 75.0f * pulso);
                    al_draw_filled_rounded_rectangle(cx - 130, cy - 6, cx + 130, cy + 46, 10, 10, al_map_rgba(255, 215, 0, (unsigned char)(60.0f * pulso)));
                    al_draw_rounded_rectangle(cx - 130, cy - 6, cx + 130, cy + 46, 10, 10, al_map_rgba(255, 215, 0, ga), 3);
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx - 2, cy, ALLEGRO_ALIGN_CENTER, "JOGAR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx + 2, cy, ALLEGRO_ALIGN_CENTER, "JOGAR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx, cy - 2, ALLEGRO_ALIGN_CENTER, "JOGAR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx, cy + 2, ALLEGRO_ALIGN_CENTER, "JOGAR");
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), cx, cy, ALLEGRO_ALIGN_CENTER, "JOGAR");
                }
                else
                    al_draw_text(fonte, al_map_rgba(180, 180, 180, 200), cx, cy, ALLEGRO_ALIGN_CENTER, "JOGAR");
            }

            /* &#9472;&#9472; botão SAIR &#9472;&#9472; */
            {
                int sel = (opcao == MENU_SAIR);
                float cx = LARGURA / 1.6f, cy = 990.0f;
                if (sel)
                {
                    float pulso = 0.7f + 0.3f * sinf((float)t * 6.0f);
                    unsigned char ga = (unsigned char)(180.0f + 75.0f * pulso);
                    al_draw_filled_rounded_rectangle(cx - 110, cy - 6, cx + 110, cy + 46, 10, 10, al_map_rgba(255, 60, 60, (unsigned char)(60.0f * pulso)));
                    al_draw_rounded_rectangle(cx - 110, cy - 6, cx + 110, cy + 46, 10, 10, al_map_rgba(255, 80, 80, ga), 3);
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx - 2, cy, ALLEGRO_ALIGN_CENTER, "SAIR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx + 2, cy, ALLEGRO_ALIGN_CENTER, "SAIR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx, cy - 2, ALLEGRO_ALIGN_CENTER, "SAIR");
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), cx, cy + 2, ALLEGRO_ALIGN_CENTER, "SAIR");
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), cx, cy, ALLEGRO_ALIGN_CENTER, "SAIR");
                }
                else
                    al_draw_text(fonte, al_map_rgba(180, 180, 180, 200), cx, cy, ALLEGRO_ALIGN_CENTER, "SAIR");
            }

            al_flip_display();
            opcao_ant = opcao;
        }
    }
    (void)opcao_ant;
}

/* ================================================================== */
/*  HUD  VIDAS / ESTAMINA / SANIDADE                                  */
/* ================================================================== */
void desenhar_vidas(VidaStatus *vidas, ALLEGRO_BITMAP *coracao)
{
    int vivas = contar_vidas(vidas);
    float pct = (float)vivas / MAX_VIDAS;
    ALLEGRO_COLOR cor = (vivas >= 4)   ? al_map_rgb(0, 200, 0)
                        : (vivas >= 2) ? al_map_rgb(220, 200, 0)
                                       : al_map_rgb(200, 0, 0);
    float bx = 290, by = 940, bw = 320, bh = 63;
    al_draw_filled_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(40, 40, 40));
    al_draw_filled_rectangle(bx, by, bx + bw * pct, by + bh, cor);
    al_draw_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(255, 255, 255), 2);
    al_draw_scaled_bitmap(coracao, 0, 0, al_get_bitmap_width(coracao), al_get_bitmap_height(coracao),
                          120, 813, 540, 340, 0);
}

void desenhar_estamina(float estamina, ALLEGRO_BITMAP *spr)
{
    float pct = estamina / MAX_ESTAMINA;
    ALLEGRO_COLOR cor = (estamina > 0) ? al_map_rgb(30, 144, 255) : al_map_rgb(80, 80, 80);
    float bx = 782, by = 940, bw = 325, bh = 63;
    al_draw_filled_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(40, 40, 40));
    al_draw_filled_rectangle(bx, by, bx + bw * pct, by + bh, cor);
    al_draw_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(255, 255, 255), 2);
    al_draw_scaled_bitmap(spr, 0, 0, al_get_bitmap_width(spr), al_get_bitmap_height(spr),
                          625, 800, 540, 340, 0);
}

void desenhar_sanidade(Sanidade *san, ALLEGRO_BITMAP *spr)
{
    float pct = san->valor / MAX_SANIDADE;
    unsigned char g = (unsigned char)(20 * pct), b = g;
    ALLEGRO_COLOR cor = (san->valor > 0) ? al_map_rgb(180, g, b) : al_map_rgb(80, 80, 80);
    float bx = 90, by = 1050, bw = 30, bh = 230, fh = bh * pct, topo = by - bh;
    al_draw_filled_rectangle(bx, topo, bx + bw, by, al_map_rgb(40, 40, 40));
    al_draw_filled_rectangle(bx, by - fh, bx + bw, by, cor);
    al_draw_rectangle(bx, topo, bx + bw, by, al_map_rgb(255, 255, 255), 2);
    if (spr)
        al_draw_scaled_bitmap(spr, 0, 0, al_get_bitmap_width(spr), al_get_bitmap_height(spr), 5, 780, 200, 300, 0);
}

/* ================================================================== */
/*  HUD  TEMPO / HORDA / ATAQUES / DASH                               */
/* ================================================================== */
void desenhar_hud_tempo(ALLEGRO_FONT *f, const char *txt)
{
    al_draw_filled_rounded_rectangle(14, 14, 314, 72, 8, 8, al_map_rgba(0, 0, 0, 120));
    al_draw_filled_rounded_rectangle(10, 10, 310, 68, 8, 8, al_map_rgb(101, 60, 20));
    al_draw_rounded_rectangle(10, 10, 310, 68, 8, 8, al_map_rgb(218, 165, 32), 3);
    al_draw_rounded_rectangle(14, 14, 306, 64, 6, 6, al_map_rgba(255, 210, 80, 80), 1);
    al_draw_text(f, al_map_rgba(0, 0, 0, 180), 22, 22, 0, txt);
    al_draw_text(f, al_map_rgb(255, 215, 0), 20, 20, 0, txt);
}

void desenhar_hud_horda(ALLEGRO_FONT *f, Horda *h)
{
    char buf[64];
    sprintf(buf, "Zumbis: %d / %d", h->total_mortos, TOTAL_ZUMBIS_FASE);
    float cx = LARGURA / 2.0f, by_ = 10, bh_ = 58, bw_ = 280;
    al_draw_filled_rounded_rectangle(cx - bw_ / 2 + 4, by_ + 4, cx + bw_ / 2 + 4, by_ + bh_ + 4, 8, 8, al_map_rgba(0, 0, 0, 120));
    al_draw_filled_rounded_rectangle(cx - bw_ / 2, by_, cx + bw_ / 2, by_ + bh_, 8, 8, al_map_rgb(101, 60, 20));
    al_draw_rounded_rectangle(cx - bw_ / 2, by_, cx + bw_ / 2, by_ + bh_, 8, 8, al_map_rgb(218, 165, 32), 3);
    al_draw_text(f, al_map_rgba(0, 0, 0, 180), cx + 2, by_ + 12 + 2, ALLEGRO_ALIGN_CENTER, buf);
    al_draw_text(f, al_map_rgb(255, 215, 0), cx, by_ + 12, ALLEGRO_ALIGN_CENTER, buf);
}

void desenhar_hud_ataque(ALLEGRO_BITMAP *at1, ALLEGRO_BITMAP *at2, ALLEGRO_BITMAP *at3,
                         ALLEGRO_BITMAP *hab1, ALLEGRO_BITMAP *hab2, ALLEGRO_BITMAP *hab3,
                         int selecao, ALLEGRO_FONT *fonte_hud)
{
    float bx = 1480.0f, by = 930.0f, tam = 90.0f, pad = 18.0f;
    ALLEGRO_BITMAP *sprites[3] = {hab1, hab2, hab3};
    const char *nomes[3] = {"", "", ""};
    for (int i = 0; i < 3; i++)
    {
        float sx = bx + i * (tam + pad), sy = by;
        int sel = (i + 1 == selecao);
        al_draw_filled_rounded_rectangle(sx + 3, sy + 3, sx + tam + 3, sy + tam + 3, 6, 6, al_map_rgba(0, 0, 0, 120));
        al_draw_filled_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6,
                                         sel ? al_map_rgb(218, 165, 32) : al_map_rgb(60, 35, 10));
        al_draw_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6, al_map_rgb(218, 165, 32), sel ? 3 : 1);
        float ip = -14.0f;
        al_draw_scaled_bitmap(sprites[i], 0, 0, al_get_bitmap_width(sprites[i]), al_get_bitmap_height(sprites[i]),
                              sx + ip, sy + ip, tam - ip * 1.6f, tam - ip * 1.6f, 0);
        al_draw_text(fonte_hud, sel ? al_map_rgb(255, 215, 0) : al_map_rgb(180, 180, 180),
                     sx + tam / 2.0f, sy + tam + 4, ALLEGRO_ALIGN_CENTER, nomes[i]);
        if (sel)
        {
            al_draw_filled_circle(sx + tam - 10, sy + 10, 9, al_map_rgb(0, 0, 0));
            al_draw_circle(sx + tam - 10, sy + 10, 9, al_map_rgb(218, 165, 32), 1);
            al_draw_text(fonte_hud, al_map_rgb(255, 235, 0), sx + tam - 10, sy + 2, ALLEGRO_ALIGN_CENTER, "K");
        }
    }
    (void)at1;
    (void)at2;
    (void)at3;
}

void desenhar_hud_dash_fuga(Jogador *jog, ALLEGRO_FONT *fonte_hud)
{
    double elapsed = al_get_time() - jog->dash_fuga_ultimo;
    float cd_pct = (elapsed >= DASH_FUGA_DELAY) ? 1.0f : (float)(elapsed / DASH_FUGA_DELAY);
    int pode = (jog->estamina >= DASH_FUGA_CUSTO) && cd_pct >= 1.0f;
    float bx = 1805.0f, by = 930.0f, bw = 90.0f, bh = 88.0f;
    al_draw_filled_rounded_rectangle(bx + 3, by + 3, bx + bw + 3, by + bh + 3, 6, 6, al_map_rgba(0, 0, 0, 120));
    al_draw_filled_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6,
                                     pode ? al_map_rgb(0, 160, 220) : al_map_rgb(30, 30, 60));
    al_draw_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6,
                              pode ? al_map_rgb(0, 220, 255) : al_map_rgb(80, 80, 120), pode ? 3 : 1);
    if (cd_pct < 1.0f)
    {
        al_draw_filled_rectangle(bx + 4, by + bh - 10, bx + 4 + (bw - 8) * cd_pct, by + bh - 4, al_map_rgb(0, 200, 255));
        al_draw_rectangle(bx + 4, by + bh - 10, bx + bw - 4, by + bh - 4, al_map_rgb(0, 100, 150), 1);
    }
    al_draw_text(fonte_hud, pode ? al_map_rgb(255, 255, 255) : al_map_rgb(100, 100, 140),
                 bx + bw / 2.0f, by + 22.0f, ALLEGRO_ALIGN_CENTER, "DASH");
    al_draw_text(fonte_hud, pode ? al_map_rgb(0, 255, 255) : al_map_rgb(80, 80, 120),
                 bx + bw / 2.0f, by + 42.0f, ALLEGRO_ALIGN_CENTER, "[E]");
}

void desenhar_hud_carga_atk2(Jogador *jog, ALLEGRO_FONT *fonte_hud)
{
    (void)fonte_hud;
    if (!jog->carregando_atk2)
        return;
    float draw_x = jog->mov.x - HITBOX_OFFSET_X;
    float draw_y = jog->mov.y - HITBOX_OFFSET_Y;
    float bw = 60.0f, bh = 8.0f;
    float bx = draw_x + DRAW_W / 2.0f - bw / 2.0f, by = draw_y + 20.0f;
    al_draw_filled_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(40, 40, 40));
    ALLEGRO_COLOR cor = al_map_rgb(30, 144, 255);
    al_draw_filled_rectangle(bx, by, bx + bw * jog->carga_atk2_pct, by + bh, cor);
    al_draw_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(255, 255, 255), 1);
}

void desenhar_roda_habilidade(ALLEGRO_BITMAP *at1, ALLEGRO_BITMAP *at2, ALLEGRO_BITMAP *at3,
                              ALLEGRO_BITMAP *hab1, ALLEGRO_BITMAP *hab2, ALLEGRO_BITMAP *hab3,
                              int selecao, ALLEGRO_FONT *fonte_hud)
{
    float cx = LARGURA / 2.0f, cy = ALTURA / 2.0f, r_ext = 175.0f, r_int = 58.0f;
    al_draw_filled_rectangle(0, 0, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 140));
    al_draw_text(fonte_hud, al_map_rgba(0, 0, 0, 200), cx + 2, cy - r_ext - 52 + 2, ALLEGRO_ALIGN_CENTER, "Selecionar Ataque");
    al_draw_text(fonte_hud, al_map_rgb(255, 215, 0), cx, cy - r_ext - 52, ALLEGRO_ALIGN_CENTER, "Selecionar Ataque");
    const char *nomes[3] = {"", "", ""};
    ALLEGRO_BITMAP *sprites[3] = {hab1, hab2, hab3};
    float base = -(float)ALLEGRO_PI / 2.0f, step = 2.0f * (float)ALLEGRO_PI / 3.0f;
    for (int i = 0; i < 3; i++)
    {
        float a0 = base + i * step, amid = a0 + step / 2.0f;
        int sel = (i + 1 == selecao);
        ALLEGRO_COLOR cf = sel ? al_map_rgba(218, 165, 32, 210) : al_map_rgba(60, 35, 10, 185);
        al_draw_filled_pieslice(cx, cy, r_ext, a0, step, cf);
        al_draw_pieslice(cx, cy, r_ext, a0, step, al_map_rgb(218, 165, 32), sel ? 4 : 2);
        float dist = r_int + (r_ext - r_int) * 0.55f;
        float ix = cx + cosf(amid) * dist, iy = cy + sinf(amid) * dist;
        float itam = sel ? 200.0f : 150.0f;
        al_draw_scaled_bitmap(sprites[i], 0, 0, al_get_bitmap_width(sprites[i]), al_get_bitmap_height(sprites[i]),
                              ix - itam / 2.0f, iy - itam / 2.0f, itam, itam, 0);
        float tx = cx + cosf(amid) * (r_ext + 32.0f), ty = cy + sinf(amid) * (r_ext + 32.0f);
        al_draw_text(fonte_hud, sel ? al_map_rgb(255, 215, 0) : al_map_rgb(200, 200, 200),
                     tx, ty - 14, ALLEGRO_ALIGN_CENTER, nomes[i]);
    }
    al_draw_filled_circle(cx, cy, r_int, al_map_rgb(28, 14, 4));
    al_draw_circle(cx, cy, r_int, al_map_rgb(218, 165, 32), 2);
    al_draw_text(fonte_hud, al_map_rgba(0, 0, 0, 180), cx + 40, cy + r_ext + 20, ALLEGRO_ALIGN_CENTER,
                 "Q / E para navegar   |   Solte K para confirmar");
    al_draw_text(fonte_hud, al_map_rgb(220, 220, 220), cx + 40, cy + r_ext + 18, ALLEGRO_ALIGN_CENTER,
                 "Q / E para navegar   |   Solte K para confirmar");
    (void)at1;
    (void)at2;
    (void)at3;
}

/* ================================================================== */
/*  POÇÃO                                                               */
/* ================================================================== */
void pocao_tentar_spawn(Pocao *p, VidaStatus *vidas)
{
    if (p->ativa || contar_vidas(vidas) != 1)
        return;
    p->x = (float)(POCAO_SPAWN_X_MIN + rand() % (POCAO_SPAWN_X_MAX - POCAO_SPAWN_X_MIN));
    p->y = (float)POCAO_SPAWN_Y;
    p->vel_y = 0.0f;
    p->no_chao = 0;
    p->ativa = 1;
    p->coletada = 0;
}

static bool pocao_piso_abaixo(ALLEGRO_BITMAP *mapa, float x, float y)
{
    int foot = (int)(y + POCAO_ALTURA) + 1;
    int left = (int)(x + (POCAO_LARGURA - POCAO_HITBOX_W) / 2.0f);
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
    if (!p->no_chao)
    {
        p->vel_y += POCAO_GRAVIDADE;
        if (p->vel_y > POCAO_MAX_QUEDA)
            p->vel_y = POCAO_MAX_QUEDA;
        float ny = p->y + p->vel_y;
        if (pocao_piso_abaixo(jog_mapa_ptr, p->x, ny))
        {
            while (!pocao_piso_abaixo(jog_mapa_ptr, p->x, p->y + 1.0f))
                p->y += 1.0f;
            p->vel_y = 0.0f;
            p->no_chao = 1;
        }
        else
            p->y = ny;
        if (p->y > ALTURA + 100)
        {
            p->ativa = 0;
            return;
        }
    }
    if (p->x < jog->mov.x + HITBOX_W && p->x + POCAO_HITBOX_W > jog->mov.x &&
        p->y < jog->mov.y + HITBOX_H && p->y + POCAO_HITBOX_H > jog->mov.y)
    {
        restaurar_vidas(vidas, POCAO_RECUPERA_VIDAS);
        tocar(sons->pocao_som);
        p->ativa = 0;
        p->coletada = 1;
    }
}

void pocao_desenhar(Pocao *p, ALLEGRO_BITMAP *spr)
{
    if (!p->ativa || !spr)
        return;
    double t = al_get_time();
    float brilho = 0.5f + 0.5f * sinf((float)t * 4.0f);
    unsigned char ga = (unsigned char)(60.0f + 80.0f * brilho);
    al_draw_filled_circle(p->x + POCAO_LARGURA / 2.0f, p->y + POCAO_ALTURA / 2.0f,
                          POCAO_LARGURA * 0.7f, al_map_rgba(100, 220, 100, ga));
    al_draw_scaled_bitmap(spr, 0, 0, al_get_bitmap_width(spr), al_get_bitmap_height(spr),
                          p->x, p->y, POCAO_LARGURA, POCAO_ALTURA, 0);
}

/* ================================================================== */
/*  RANKING / GAME OVER                                                 */
/* ================================================================== */
void ordenar_ranking(float r[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (r[j] > r[j + 1])
            {
                float t = r[j];
                r[j] = r[j + 1];
                r[j + 1] = t;
            }
}

int busca_binaria(float r[], int n, float v)
{
    int lo = 0, hi = n - 1, mid;
    while (lo <= hi)
    {
        mid = (lo + hi) / 2;
        if (v < r[mid])
            hi = mid - 1;
        else
            lo = mid + 1;
    }
    return lo;
}

void carregar_ranking(Temporizador *t)
{
    FILE *f = fopen("ranking.txt", "r");
    if (!f)
        return;
    while (fscanf(f, "%f", &t->ranking[t->quantidade_scores]) == 1)
    {
        t->quantidade_scores++;
        if (t->quantidade_scores >= 10)
            break;
    }
    fclose(f);
    ordenar_ranking(t->ranking, t->quantidade_scores);
}

void salvar_ranking(Temporizador *t)
{
    FILE *f = fopen("ranking.txt", "w");
    if (!f)
        return;
    for (int i = 0; i < t->quantidade_scores; i++)
        fprintf(f, "%.2f\n", t->ranking[i]);
    fclose(f);
}

void tela_ranking(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                  ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                  Temporizador *tempo, double tempo_final)
{
    int pos = busca_binaria(tempo->ranking, tempo->quantidade_scores, (float)tempo_final);
    if (tempo->quantidade_scores < 10)
    {
        for (int i = tempo->quantidade_scores; i > pos; i--)
            tempo->ranking[i] = tempo->ranking[i - 1];
        tempo->ranking[pos] = (float)tempo_final;
        tempo->quantidade_scores++;
    }
    else if (pos < 10)
    {
        for (int i = 9; i > pos; i--)
            tempo->ranking[i] = tempo->ranking[i - 1];
        tempo->ranking[pos] = (float)tempo_final;
    }
    salvar_ranking(tempo);

    ALLEGRO_EVENT ev;
    while (1)
    {
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return;
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return;
        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;

        al_clear_to_color(al_map_rgb(15, 10, 5));
        float pw = 700, ph = 680, px_ = LARGURA / 2.0f - pw / 2.0f, py_ = ALTURA / 2.0f - ph / 2.0f;
        al_draw_filled_rounded_rectangle(px_ + 6, py_ + 6, px_ + pw + 6, py_ + ph + 6, 14, 14, al_map_rgba(0, 0, 0, 160));
        al_draw_filled_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(50, 28, 8));
        al_draw_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(218, 165, 32), 4);
        al_draw_text(fonte, al_map_rgb(255, 215, 0), LARGURA / 2.0f, py_ + 20, ALLEGRO_ALIGN_CENTER, "FASE CONCLUIDA!");
        char buf[80];
        sprintf(buf, "Seu tempo: %.2f s", tempo_final);
        al_draw_text(fonte_hud, al_map_rgb(255, 255, 180), LARGURA / 2.0f, py_ + 80, ALLEGRO_ALIGN_CENTER, buf);
        al_draw_text(fonte_hud, al_map_rgb(218, 165, 32), LARGURA / 2.0f, py_ + 148, ALLEGRO_ALIGN_CENTER, "TOP 10");
        for (int i = 0; i < tempo->quantidade_scores; i++)
        {
            char linha[64];
            sprintf(linha, "%d.   %.2f s", i + 1, tempo->ranking[i]);
            ALLEGRO_COLOR cor = (fabsf(tempo->ranking[i] - (float)tempo_final) < 0.005f)
                                    ? al_map_rgb(255, 215, 0)
                                    : al_map_rgb(220, 220, 220);
            al_draw_text(fonte_hud, cor, LARGURA / 2.0f, py_ + 195 + i * 44, ALLEGRO_ALIGN_CENTER, linha);
        }
        al_draw_text(fonte_hud, al_map_rgb(160, 160, 160), LARGURA / 2.0f, py_ + ph - 52, ALLEGRO_ALIGN_CENTER,
                     "Pressione ENTER para sair");
        al_flip_display();
    }
}

int tela_game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                   ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud, const char *motivo)
{
    al_flush_event_queue(queue);
    ALLEGRO_EVENT ev;
    int opcao = 0;
    while (1)
    {
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return 0;
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT || ev.keyboard.keycode == ALLEGRO_KEY_A)
                opcao = 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT || ev.keyboard.keycode == ALLEGRO_KEY_D)
                opcao = 1;
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
                return (opcao == 0) ? 1 : 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return 0;
        }
        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;
        al_clear_to_color(al_map_rgb(10, 5, 5));
        float pw = 910, ph = 340, px_ = LARGURA / 2.0f - pw / 2.0f, py_ = ALTURA / 2.0f - ph / 2.0f;
        al_draw_filled_rounded_rectangle(px_ + 6, py_ + 6, px_ + pw + 6, py_ + ph + 6, 14, 14, al_map_rgba(0, 0, 0, 180));
        al_draw_filled_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(60, 10, 10));
        al_draw_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(200, 30, 30), 4);
        al_draw_text(fonte, al_map_rgb(255, 60, 60), LARGURA / 2.0f, py_ + 28, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_line(px_ + 40, py_ + 88, px_ + pw - 40, py_ + 88, al_map_rgb(200, 30, 30), 2);
        al_draw_text(fonte_hud, al_map_rgb(255, 200, 200), LARGURA / 2.0f, py_ + 102, ALLEGRO_ALIGN_CENTER, motivo);
        al_draw_line(px_ + 40, py_ + 158, px_ + pw - 40, py_ + 158, al_map_rgb(120, 20, 20), 1);
        const char *tr = (opcao == 0) ? "> REINICIAR <" : "REINICIAR";
        const char *ts = (opcao == 1) ? "> SAIR <" : "SAIR";
        al_draw_text(fonte, (opcao == 0) ? al_map_rgb(255, 215, 0) : al_map_rgb(180, 180, 180),
                     LARGURA / 2.0f - 160, py_ + 186, ALLEGRO_ALIGN_CENTER, tr);
        al_draw_text(fonte, (opcao == 1) ? al_map_rgb(255, 215, 0) : al_map_rgb(180, 180, 180),
                     LARGURA / 2.0f + 160, py_ + 186, ALLEGRO_ALIGN_CENTER, ts);
        al_draw_text(fonte_hud, al_map_rgb(130, 130, 130), LARGURA / 2.0f, py_ + ph - 46, ALLEGRO_ALIGN_CENTER,
                     "A/D ou Setas para navegar  |  ENTER para confirmar");
        al_flip_display();
    }
}

/* ================================================================== */
/*  MAPA DE COLISÃO                                                     */
/* ================================================================== */
void gerar_mapa_colisao(ALLEGRO_BITMAP *mapa)
{
    al_lock_bitmap(mapa, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
    for (int y = 0; y < al_get_bitmap_height(mapa); y++)
        for (int x = 0; x < al_get_bitmap_width(mapa); x++)
        {
            ALLEGRO_COLOR c = al_get_pixel(mapa, x, y);
            unsigned char r, g, b;
            al_unmap_rgb(c, &r, &g, &b);
            colisao[y][x] = ((r + g + b) / 3 < 120);
        }
    al_unlock_bitmap(mapa);
}

/* ================================================================== */
/*  DESENHO DO SAMURAI                                                  */
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
        if (f >= FRAMES_HURT)
            f = FRAMES_HURT - 1;
        al_draw_scaled_bitmap(spr->hurt, 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
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
        al_draw_scaled_bitmap(spr->dead, 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        jog->frame_dead += 0.06f;
        break;
    }
    case SAM_ATTACK:
    {
        int mf = (jog->tipo_ataque == 1) ? 6 : (jog->tipo_ataque == 2) ? 4
                                                                       : 3;
        int f = (int)jog->frame_ataque;
        if (f >= mf)
            f = mf - 1;
        ALLEGRO_BITMAP *atk = (jog->tipo_ataque == 1) ? spr->attack1 : (jog->tipo_ataque == 2) ? spr->attack2
                                                                                               : spr->attack3;
        al_draw_scaled_bitmap(atk, 128 * f, 0, 128, 128, draw_x - 10, draw_y - 1, ATTACK_DRAW_W, ATTACK_DRAW_H, jog->direcao);
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
        float off = (jog->direcao == 0) ? -18.0f : 18.0f;
        al_draw_tinted_scaled_bitmap(spr->run, al_map_rgba(180, 240, 255, 220), 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        al_draw_tinted_scaled_bitmap(spr->run, al_map_rgba(100, 200, 255, 80), 128 * f, 0, 128, 128, draw_x + off, draw_y + 4, DRAW_W, DRAW_H, jog->direcao);
        al_draw_tinted_scaled_bitmap(spr->run, al_map_rgba(100, 200, 255, 40), 128 * f, 0, 128, 128, draw_x + off * 2.0f, draw_y + 8, DRAW_W, DRAW_H, jog->direcao);
        jog->dash_fuga_frame += 0.5f;
        break;
    }
    case SAM_CHARGING:
    {
        int f = (int)jog->frame % FRAMES_IDLE;
        float intensidade = 0.6f + 0.8f * jog->carga_atk2_pct;
        float shake_x = sinf((float)al_get_time() * 30.0f) * intensidade;
        al_draw_scaled_bitmap(spr->idle, 128 * f, 0, 128, 128,
                              draw_x + shake_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        break;
    }
    case SAM_JUMP:
    {
        int f = (int)jog->frame % FRAMES_JUMP;
        al_draw_scaled_bitmap(spr->jump, 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        break;
    }
    case SAM_RUN:
    {
        int f = (int)jog->frame % FRAMES_RUN;
        al_draw_scaled_bitmap(spr->run, 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        break;
    }
    default:
    {
        int f = (int)jog->frame % FRAMES_IDLE;
        al_draw_scaled_bitmap(spr->idle, 128 * f, 0, 128, 128, draw_x, draw_y, DRAW_W, DRAW_H, jog->direcao);
        break;
    }
    }
}

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

/* ================================================================== */
/*  MAIN                                                                */
/* ================================================================== */
int main(void)
{
    srand((unsigned)time(NULL));

    if (!al_init())
    {
        puts("Erro al_init");
        return 1;
    }
    if (!al_init_primitives_addon())
    {
        puts("Erro primitives");
        return 1;
    }
    if (!al_install_keyboard())
    {
        puts("Erro keyboard");
        return 1;
    }
    if (!al_init_image_addon())
    {
        puts("Erro image");
        return 1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    if (!al_install_audio())
    {
        puts("Erro al_install_audio");
        return 1;
    }
    if (!al_init_acodec_addon())
    {
        puts("Erro al_init_acodec");
        return 1;
    }
    if (!al_reserve_samples(32))
    {
        puts("Erro al_reserve_samples");
        return 1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    if (!display || !queue || !timer)
    {
        puts("Erro init");
        return 1;
    }

    ALLEGRO_FONT *fonte = al_load_ttf_font("assets/PublicPixel.ttf", 30, 0);
    ALLEGRO_FONT *fonte_hud = al_load_ttf_font("assets/PublicPixel.ttf", 18, 0);
    if (!fonte)
        fonte = al_create_builtin_font();
    if (!fonte_hud)
        fonte_hud = al_create_builtin_font();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    Sons sons = carregar_sons();

    ALLEGRO_BITMAP *bg_menu = al_load_bitmap("assets/cenarios/inicio.png");
    if (!bg_menu)
    {
        puts("Erro bg_menu");
        return 1;
    }

    al_start_timer(timer);
    tocar_musica_fundo(&sons);

    OpcaoMenu escolha = executar_menu(queue, timer, bg_menu, fonte, fonte_hud, &sons);
    if (escolha == MENU_SAIR)
    {
        al_destroy_bitmap(bg_menu);
        destruir_sons(&sons);
        al_destroy_font(fonte_hud);
        al_destroy_font(fonte);
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
        al_destroy_display(display);
        return 0;
    }
    al_flush_event_queue(queue);

    /* ?? Bitmaps de jogo ?? */
    ALLEGRO_BITMAP *bg = al_load_bitmap("assets/cenarios/background2.png");
    ALLEGRO_BITMAP *mapa = al_load_bitmap("assets/cenarios/colisao2.png");

    SamuraiSprites sam_spr;
    sam_spr.idle = al_load_bitmap("assets/sprites/Samurai/Idle.png");
    sam_spr.run = al_load_bitmap("assets/sprites/Samurai/Run.png");
    sam_spr.jump = al_load_bitmap("assets/sprites/Samurai/Jump.png");
    sam_spr.attack1 = al_load_bitmap("assets/sprites/Samurai/Attack_1.png");
    sam_spr.attack2 = al_load_bitmap("assets/sprites/Samurai/Attack_2.png");
    sam_spr.attack3 = al_load_bitmap("assets/sprites/Samurai/Attack_3.png");
    sam_spr.hurt = al_load_bitmap("assets/sprites/Samurai/Hurt.png");
    sam_spr.dead = al_load_bitmap("assets/sprites/Samurai/Dead.png");

    ZumbiSprites zum_spr;
    zum_spr.walk = al_load_bitmap("assets/sprites/Zombies/Walk.png");
    zum_spr.run = al_load_bitmap("assets/sprites/Zombies/Run.png");
    zum_spr.attack1 = al_load_bitmap("assets/sprites/Zombies/Attack_1.png");
    zum_spr.attack2 = al_load_bitmap("assets/sprites/Zombies/Attack_2.png");
    zum_spr.attack3 = al_load_bitmap("assets/sprites/Zombies/Attack_3.png");
    zum_spr.bite = al_load_bitmap("assets/sprites/Zombies/Bite.png");
    zum_spr.hurt = al_load_bitmap("assets/sprites/Zombies/Hurt.png");
    zum_spr.dead = al_load_bitmap("assets/sprites/Zombies/Dead.png");
    zum_spr.idle = al_load_bitmap("assets/sprites/Zombies/Idle.png");

    /* ?? Zumbi ácido ?? */
    ZumbiAcidoSprites zum_acido_spr;
    zum_acido_spr.walk = al_load_bitmap("assets/sprites/Zumbi-acido/Walk.png");
    zum_acido_spr.attack = al_load_bitmap("assets/sprites/Zumbi-acido/Attack.png");
    zum_acido_spr.hurt = al_load_bitmap("assets/sprites/Zumbi-acido/Hurt.png");
    zum_acido_spr.dead = al_load_bitmap("assets/sprites/Zumbi-acido/Dead.png");
    zum_acido_spr.idle = al_load_bitmap("assets/sprites/Zumbi-acido/Idle.png");

    /* ?? Projétil de ácido ?? */
    ALLEGRO_BITMAP *spr_acido_projetil = al_load_bitmap("assets/itens/acido.png");

    /* ?? Frames da explosão ácida ?? */
    ALLEGRO_BITMAP *explosao_acida_frames[FRAMES_EXPLOSAO_ACIDA];
    for (int i = 0; i < FRAMES_EXPLOSAO_ACIDA; i++)
    {
        char path[64];
        sprintf(path, "assets/itens/explosao_acida/Explosion_%d.png", i + 1);
        explosao_acida_frames[i] = al_load_bitmap(path);
    }

    ALLEGRO_BITMAP *coracao = al_load_bitmap("assets/itens/vida.png");
    ALLEGRO_BITMAP *spr_estamina = al_load_bitmap("assets/itens/estamina.png");
    ALLEGRO_BITMAP *spr_sanidade = al_load_bitmap("assets/itens/sanidade.png");
    ALLEGRO_BITMAP *habilidade1 = al_load_bitmap("assets/itens/Habilidade1.png");
    ALLEGRO_BITMAP *habilidade2 = al_load_bitmap("assets/itens/Habilidade2.png");
    ALLEGRO_BITMAP *habilidade3 = al_load_bitmap("assets/itens/Habilidade3.png");
    ALLEGRO_BITMAP *spr_pocao = al_load_bitmap("assets/itens/pocao.png");

    if (!bg || !mapa ||
        !sam_spr.idle || !sam_spr.run || !sam_spr.jump ||
        !sam_spr.attack1 || !sam_spr.attack2 || !sam_spr.attack3 ||
        !sam_spr.hurt || !sam_spr.dead ||
        !zum_spr.walk || !zum_spr.run ||
        !zum_spr.attack1 || !zum_spr.attack2 || !zum_spr.attack3 ||
        !zum_spr.bite || !zum_spr.hurt || !zum_spr.dead || !zum_spr.idle ||
        !zum_acido_spr.walk || !zum_acido_spr.attack || !zum_acido_spr.hurt ||
        !zum_acido_spr.dead || !zum_acido_spr.idle || !spr_acido_projetil ||
        !coracao || !spr_estamina || !habilidade1 || !habilidade2 || !habilidade3)
    {
        puts("Erro ao carregar bitmaps");
        return 1;
    }
    if (!spr_pocao)
        puts("Aviso: assets/itens/pocao.png nao encontrado");

    gerar_mapa_colisao(mapa);
    jog_mapa_ptr = mapa;

    /* ?? Estado inicial do jogador ?? */
    Jogador jogador = {0};
    jogador.mov.x = 60;
    jogador.mov.y = 253;
    jogador.tipo_ataque = 1;
    jogador.estamina = MAX_ESTAMINA;
    jogador.estado = SAM_IDLE;
    jogador.hurt_inicio = -999.0;
    jogador.ultimo_ataque = -999.0;
    jogador.dash_fuga_ultimo = -999.0;

    Sanidade sanidade = {MAX_SANIDADE, 0, 0, 0, 0.0, 0};

    Horda horda;
    horda_init(&horda);
    for (int iz = 0; iz < MAX_ZUMBIS_TELA; iz++)
        horda.pool[iz].ja_stunado_no_dash = 0;

    /* ?? Zera projéteis e explosões ?? */
    for (int ip = 0; ip < MAX_PROJETEIS_ACIDO; ip++)
        g_projeteis_acido[ip].ativo = 0;
    for (int ie = 0; ie < MAX_EXPLOSOES_ACIDAS; ie++)
        g_explosoes_acidas[ie].ativo = 0;

    Pocao pocao = {0};
    double pocao_ultimo_check = 0.0;

    Temporizador tempo = {0};
    tempo.inicio = al_get_time();
    tempo.ativo = 1;
    carregar_ranking(&tempo);

    VidaStatus *vetor_vidas = malloc(MAX_VIDAS * sizeof(VidaStatus));
    for (int i = 0; i < MAX_VIDAS; i++)
    {
        vetor_vidas[i].ativa = 1;
        strcpy(vetor_vidas[i].status, "Inteira");
    }

    int lm = ALTURA / 100 + 1, cm = LARGURA / 100 + 1;
    Particula *matriz_dec = criar_matriz_decorativa(lm, cm);

    int pausado = 0;
    double pausa_inicio = 0;
    int roda_aberta = 0, roda_selecao = 1;
    sanidade.ultimo_abate = al_get_time();

    int esc_ant = 0, k_ant = 0, j_ant = 0, q_ant = 0, e_ant = 0;
    int som_finish_tocado = 0, som_gameover_tocado = 0;
    int rodando = 1;

    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE state;

    /* ================================================================
       LOOP PRINCIPAL
       ================================================================ */
    while (rodando)
    {
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            rodando = 0;
            break;
        }
        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;

        al_get_keyboard_state(&state);

        /* ?? ESC: pausa ?? */
        int esc_now = al_key_down(&state, ALLEGRO_KEY_ESCAPE);
        if (esc_now && !esc_ant &&
            !sanidade.game_over && !horda.fase_concluida &&
            jogador.estado != SAM_DEAD)
        {
            pausado = !pausado;
            if (pausado)
            {
                pausa_inicio = al_get_time();
                tocar(sons.esc_som);
            }
            else
            {
                double dt = al_get_time() - pausa_inicio;
                tempo.inicio += dt;
                jogador.ultimo_dano += dt;
                sanidade.ultimo_abate += dt;
            }
        }
        esc_ant = esc_now;

        int morte_pronta = (jogador.estado == SAM_DEAD && !jogador.morte_animando);
        int em_game_over = sanidade.game_over || morte_pronta;

        if (!pausado && !em_game_over && !horda.fase_concluida)
        {
            atualizar_sanidade(&sanidade);
            jogador.frame += 0.15f;
            jogador.movendo = 0;
            if (tempo.ativo)
                tempo.atual = al_get_time() - tempo.inicio;

            /* ?? roda de habilidades ?? */
            int k_now = al_key_down(&state, ALLEGRO_KEY_K);
            if (k_now && !k_ant)
            {
                roda_aberta = 1;
                roda_selecao = jogador.tipo_ataque;
            }
            if (!k_now && k_ant)
            {
                jogador.tipo_ataque = roda_selecao;
                roda_aberta = 0;
            }
            if (roda_aberta)
            {
                int q_now2 = al_key_down(&state, ALLEGRO_KEY_Q);
                int e_now2 = al_key_down(&state, ALLEGRO_KEY_E);
                if (q_now2 && !q_ant)
                {
                    roda_selecao--;
                    if (roda_selecao < 1)
                        roda_selecao = 3;
                }
                if (e_now2 && !e_ant)
                {
                    roda_selecao++;
                    if (roda_selecao > 3)
                        roda_selecao = 1;
                }
                q_ant = q_now2;
                e_ant = e_now2;
            }
            k_ant = k_now;

            /* ?? dash de fuga (E) ?? */
            int e_now_dash = al_key_down(&state, ALLEGRO_KEY_E);
            if (!roda_aberta)
            {
                if (e_now_dash && !e_ant &&
                    !jogador.dash_fuga_ativo &&
                    jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
                    jogador.estado != SAM_ATTACK &&
                    jogador.estamina >= DASH_FUGA_CUSTO &&
                    (al_get_time() - jogador.dash_fuga_ultimo) >= DASH_FUGA_DELAY)
                {
                    jogador.dash_fuga_ativo = 1;
                    jogador.dash_fuga_dist = DASH_FUGA_DIST;
                    jogador.dash_fuga_frame = 0.0f;
                    jogador.dash_fuga_ultimo = al_get_time();
                    jogador.estado = SAM_DASH;
                    jogador.estamina -= DASH_FUGA_CUSTO;
                    if (jogador.estamina < 0.0f)
                        jogador.estamina = 0.0f;
                    tocar(sons.dash);
                }
                e_ant = e_now_dash;
            }

            /* processa dash de fuga */
            if (jogador.dash_fuga_ativo)
            {
                float passo_fuga = 14.0f;
                if (jogador.dash_fuga_dist > 0.0f)
                {
                    float move = (jogador.dash_fuga_dist < passo_fuga) ? jogador.dash_fuga_dist : passo_fuga;
                    float dx_fuga = (jogador.direcao == 0) ? move : -move;
                    float nx_f = jogador.mov.x + dx_fuga;
                    if (!colide_mapa(mapa, nx_f, jogador.mov.y))
                        jogador.mov.x = nx_f;
                    jogador.dash_fuga_dist -= move;

                    /* stun em zumbis próximos */
                    double agora_dash = al_get_time();
                    for (int iz = 0; iz < MAX_ZUMBIS_TELA; iz++)
                    {
                        Inimigo *z = &horda.pool[iz];
                        if (!z->vivo || z->estado == ZUM_DEAD || z->stunado)
                            continue;
                        float dist_x = fabsf((z->x + ZUMBI_HBX_OFFSET_X + ZUMBI_HBX_W / 2.0f) - (jogador.mov.x + HITBOX_W / 2.0f));
                        float dist_y = fabsf((z->y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H / 2.0f) - (jogador.mov.y + HITBOX_H / 2.0f));
                        if (dist_x < 70.0f && dist_y < 70.0f)
                        {
                            float kb_dir = (z->x > jogador.mov.x) ? 1.0f : -1.0f;
                            z->x += kb_dir * 18.0f;
                            z->stunado = 1;
                            z->tempo_stun = agora_dash;
                            z->estado = ZUM_HURT;
                            z->frame = 0;
                            z->tempo_hurt = agora_dash;
                            z->dano_aplicado = 0;
                            tocar(sons.zumbi_knockback);
                        }
                    }

                    /* dash destrói projéteis de ácido próximos */
                    for (int ip = 0; ip < MAX_PROJETEIS_ACIDO; ip++)
                    {
                        ProjetilAcido *p = &g_projeteis_acido[ip];
                        if (!p->ativo || p->impactou)
                            continue;
                        float dist_x_p = fabsf(p->x - (jogador.mov.x + HITBOX_W / 2.0f));
                        float dist_y_p = fabsf(p->y - (jogador.mov.y + HITBOX_H / 2.0f));
                        if (dist_x_p < 70.0f && dist_y_p < 70.0f)
                        {
                            p->impactou = 1;
                            p->frame = 5.0f;
                            tocar(sons.impacto_espada);
                        }
                    }
                }
                if (jogador.dash_fuga_dist <= 0.0f)
                {
                    jogador.dash_fuga_ativo = 0;
                    jogador.estado = SAM_IDLE;
                }
            }

            /* ?? ataque (J) ?? */
            int j_now = al_key_down(&state, ALLEGRO_KEY_J);
            double agora_atk = al_get_time();
            double delay_atual = (jogador.tipo_ataque == 3) ? DELAY_ATAQUE_3 : DELAY_ATAQUE_12;

            /* início da carga do ataque 2 */
            if (jogador.tipo_ataque == 2 &&
                j_now && !j_ant && !jogador.atacando && !roda_aberta &&
                !jogador.dash_fuga_ativo && !jogador.carregando_atk2 &&
                jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
                jogador.estamina >= CUSTO_ATK2_MIN &&
                (agora_atk - jogador.ultimo_ataque) >= delay_atual)
            {
                jogador.carregando_atk2 = 1;
                jogador.carga_atk2_inicio = agora_atk;
                jogador.carga_atk2_pct = 0.0f;
                jogador.estado = SAM_CHARGING;
            }

            /* enquanto carrega: atualiza % e verifica soltura/estamina */
            if (jogador.carregando_atk2)
            {
                double carregando_ha = agora_atk - jogador.carga_atk2_inicio;
                float pct = (float)(carregando_ha / CARGA_ATK2_TEMPO_MAX);
                if (pct > 1.0f)
                    pct = 1.0f;
                jogador.carga_atk2_pct = pct;

                float custo_projetado = CUSTO_ATK2_MIN + (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN) * pct;
                int estamina_insuficiente = (jogador.estamina < custo_projetado);

                int soltou = (!j_now && j_ant);

                if (soltou || estamina_insuficiente)
                {
                    /* se a estamina não aguenta a carga atual, recalcula a carga
                       máxima que a estamina disponível permite */
                    if (estamina_insuficiente)
                    {
                        float pct_max_pela_estamina =
                            (jogador.estamina - CUSTO_ATK2_MIN) / (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN);
                        if (pct_max_pela_estamina < 0.0f)
                            pct_max_pela_estamina = 0.0f;
                        if (pct_max_pela_estamina > 1.0f)
                            pct_max_pela_estamina = 1.0f;
                        pct = pct_max_pela_estamina;
                    }

                    float custo_final = CUSTO_ATK2_MIN + (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN) * pct;
                    int dano_final = DANO_ATK2_MIN + (int)((DANO_ATK2_MAX - DANO_ATK2_MIN) * pct + 0.5f);

                    jogador.estamina -= custo_final;
                    if (jogador.estamina < 0.0f)
                        jogador.estamina = 0.0f;

                    jogador.dano_atk2_atual = dano_final;
                    jogador.carregando_atk2 = 0;
                    jogador.carga_atk2_pct = 0.0f;

                    jogador.atacando = 1;
                    jogador.frame_ataque = 0;
                    jogador.acertos_no_swing = 0;
                    jogador.som_ataque_tocado = 0;
                    jogador.dash_som_tocado = 0;
                    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
                        horda.pool[i].atingido_no_ataque = 0;

                    jogador.estado = SAM_ATTACK;
                    jogador.ultimo_ataque = agora_atk;
                    tocar(sons.katana12);
                }
            }

            /* ataques 1 e 3: comportamento original (instantâneo) */
            if (jogador.tipo_ataque != 2 &&
                j_now && !j_ant && !jogador.atacando && !roda_aberta &&
                !jogador.dash_fuga_ativo &&
                jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
                (agora_atk - jogador.ultimo_ataque) >= delay_atual)
            {
                int custo_est = (jogador.tipo_ataque == 3) ? (int)CUSTO_ATK3 : 1;
                if (jogador.estamina >= custo_est)
                {
                    jogador.atacando = 1;
                    jogador.frame_ataque = 0;
                    jogador.acertos_no_swing = 0;
                    jogador.som_ataque_tocado = 0;
                    jogador.dash_som_tocado = 0;
                    for (int i = 0; i < MAX_ZUMBIS_TELA; i++)
                        horda.pool[i].atingido_no_ataque = 0;
                    jogador.estamina -= custo_est;
                    if (jogador.estamina < 0)
                        jogador.estamina = 0;
                    jogador.estado = SAM_ATTACK;
                    jogador.ultimo_ataque = agora_atk;
                    if (jogador.tipo_ataque == 3)
                        tocar(sons.katana_attack3);
                    else
                        tocar(sons.katana12);
                    if (jogador.tipo_ataque == 3)
                    {
                        jogador.dash_ativo = 1;
                        jogador.dash_dist = DASH_ATK3_DIST;
                        jogador.dash_som_tocado = 1;
                        tocar(sons.dash);
                    }
                }
            }
            j_ant = j_now;

            /* dash do ataque 3 */
            if (jogador.dash_ativo && jogador.atacando)
            {
                float passo = 8.0f;
                if (jogador.dash_dist > 0.0f)
                {
                    float move = (jogador.dash_dist < passo) ? jogador.dash_dist : passo;
                    float dx_dash = (jogador.direcao == 0) ? move : -move;
                    float nx_d = jogador.mov.x + dx_dash;
                    if (!colide_mapa(mapa, nx_d, jogador.mov.y))
                        jogador.mov.x = nx_d;
                    jogador.dash_dist -= move;
                }
                if (jogador.dash_dist <= 0.0f || !jogador.atacando)
                    jogador.dash_ativo = 0;
            }

            /* ?? movimento e pulo ?? */
            if (jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
                jogador.estado != SAM_CHARGING && !jogador.dash_fuga_ativo)
            {
                float nx = jogador.mov.x;
                if (al_key_down(&state, ALLEGRO_KEY_D) && !jogador.atacando && !roda_aberta)
                {
                    nx += VELOCIDADE;
                    jogador.direcao = 0;
                    jogador.movendo = 1;
                }
                if (al_key_down(&state, ALLEGRO_KEY_A) && !jogador.atacando && !roda_aberta)
                {
                    nx -= VELOCIDADE;
                    jogador.direcao = ALLEGRO_FLIP_HORIZONTAL;
                    jogador.movendo = 1;
                }
                if (!colide_mapa(mapa, nx, jogador.mov.y))
                    jogador.mov.x = nx;

                if (al_key_down(&state, ALLEGRO_KEY_W) && jogador.no_chao &&
                    !jogador.atacando && !roda_aberta && jogador.estamina >= CUSTO_PULO)
                {
                    jogador.mov.vel_y = FORCA_PULO;
                    jogador.estamina -= CUSTO_PULO;
                    if (jogador.estamina < 0)
                        jogador.estamina = 0;
                    tocar(sons.pulo);
                }

                /* som de passos */
                if (jogador.no_chao && jogador.movendo && jogador.estado == SAM_RUN)
                {
                    double agora_sam = al_get_time();
                    if ((agora_sam - jogador.ultimo_passo_sam) >= SOM_WALK_SAM_INTERVALO)
                    {
                        jogador.ultimo_passo_sam = agora_sam;
                        tocar(sons.walk_sam);
                    }
                }
            }

            /* ?? gravidade ?? */
            jogador.mov.vel_y += GRAVIDADE;
            if (jogador.mov.vel_y > MAX_QUEDA)
                jogador.mov.vel_y = MAX_QUEDA;
            float ny = jogador.mov.y + jogador.mov.vel_y;
            if (!colide_mapa(mapa, jogador.mov.x, ny))
                jogador.mov.y = ny;
            else
                jogador.mov.vel_y = 0;
            jogador.no_chao = esta_no_chao(mapa, jogador.mov.x, jogador.mov.y);
            if (jogador.no_chao)
            {
                jogador.y_chao = jogador.mov.y;
                jogador.estamina += RECARGA_ESTAMINA;
                if (jogador.estamina > MAX_ESTAMINA)
                    jogador.estamina = MAX_ESTAMINA;
            }
            if (jogador.mov.y > ALTURA + 200)
            {
                tocar(sons.caindo);
                perder_vida(vetor_vidas);
                jogador.mov.x = 60;
                jogador.mov.y = 253;
                jogador.mov.vel_y = 0;
            }

            atualizar_estado_samurai(&jogador);

            /* ?? horda ?? */
            horda_atualizar_spawn(&horda, &sons, jogador.mov.y);
            horda_atualizar_fisica(&horda, mapa);
            horda_atualizar_movimento(&horda, &jogador, &sons, mapa);
            horda_verificar_ataque(&horda, &jogador, &sanidade, &sons);
            horda_verificar_dano_jogador(&horda, &jogador, vetor_vidas, &sons);

            /* ?? projéteis e explosões ácidas ?? */
            projeteis_acido_atualizar(mapa, &jogador, vetor_vidas, &sons);
            explosoes_acidas_atualizar(&horda, &jogador, vetor_vidas, &sanidade, &sons);

            /* ?? poção ?? */
            {
                double agora_poc = al_get_time();
                if (!pocao.ativa && (agora_poc - pocao_ultimo_check) >= 3.0)
                {
                    pocao_ultimo_check = agora_poc;
                    pocao_tentar_spawn(&pocao, vetor_vidas);
                }
                pocao_atualizar(&pocao, &jogador, vetor_vidas, &sons);
            }

            /* ?? morte do jogador ?? */
            {
                int vivas = contar_vidas(vetor_vidas);
                if (vivas == 0 && jogador.estado != SAM_DEAD)
                {
                    jogador.estado = SAM_DEAD;
                    jogador.frame_dead = 0;
                    jogador.morte_animando = 1;
                    jogador.mov.vel_y = -3.0f;
                    jogador.som_morrendo_tocado = 0;
                }
            }
            if (jogador.estado == SAM_DEAD && jogador.morte_animando && !jogador.som_morrendo_tocado)
            {
                tocar(sons.morrendo);
                jogador.som_morrendo_tocado = 1;
            }
            if (horda.fase_concluida && tempo.ativo)
            {
                tempo.fim = al_get_time();
                tempo.atual = tempo.fim - tempo.inicio;
                tempo.ativo = 0;
            }
        }

        /* física durante animação de morte */
        if (!pausado && jogador.estado == SAM_DEAD && jogador.morte_animando && !sanidade.game_over)
        {
            jogador.mov.vel_y += GRAVIDADE * 0.30f;
            if (jogador.mov.vel_y > 3.5f)
                jogador.mov.vel_y = 3.5f;
            float ny2 = jogador.mov.y + jogador.mov.vel_y;
            if (!colide_mapa(mapa, jogador.mov.x, ny2))
                jogador.mov.y = ny2;
            else
                jogador.mov.vel_y = 0;
            morte_pronta = (jogador.estado == SAM_DEAD && !jogador.morte_animando);
        }

        /* ?? DESENHO ?? */
        double t_agora = al_get_time();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(bg, 0, 0, 0);
        desenhar_matriz_fundo(matriz_dec, lm * cm, t_agora);

        pocao_desenhar(&pocao, spr_pocao);
        horda_desenhar(&horda, &zum_spr, &zum_acido_spr);
        projeteis_acido_desenhar(spr_acido_projetil);
        explosoes_acidas_desenhar(explosao_acida_frames);
        desenhar_samurai(&jogador, &sam_spr);
        desenhar_overlay_sanidade(&sanidade);

        char txt[60];
        sprintf(txt, "Tempo: %.2f s", tempo.atual);
        desenhar_hud_tempo(fonte_hud, txt);
        desenhar_hud_horda(fonte_hud, &horda);
        desenhar_vidas(vetor_vidas, coracao);
        desenhar_estamina(jogador.estamina, spr_estamina);
        desenhar_sanidade(&sanidade, spr_sanidade);
        desenhar_aviso_sanidade(&sanidade, fonte_hud, t_agora);
        desenhar_mensagem_central_insanidade(&sanidade, fonte, fonte_hud, t_agora);
        desenhar_hud_ataque(sam_spr.attack1, sam_spr.attack2, sam_spr.attack3,
                            habilidade1, habilidade2, habilidade3,
                            jogador.tipo_ataque, fonte_hud);
        desenhar_hud_dash_fuga(&jogador, fonte_hud);
        desenhar_hud_carga_atk2(&jogador, fonte_hud);

        if (pausado)
        {
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 160));
            al_draw_filled_rounded_rectangle(LARGURA / 2.0 - 310, ALTURA / 2.0 - 90,
                                             LARGURA / 2.0 + 310, ALTURA / 2.0 + 70,
                                             12, 12, al_map_rgb(101, 60, 20));
            al_draw_rounded_rectangle(LARGURA / 2.0 - 310, ALTURA / 2.0 - 90,
                                      LARGURA / 2.0 + 310, ALTURA / 2.0 + 70,
                                      12, 12, al_map_rgb(218, 165, 32), 3);
            al_draw_text(fonte, al_map_rgb(255, 215, 0),
                         LARGURA / 2.0, ALTURA / 2.0 - 75, ALLEGRO_ALIGN_CENTER, "PAUSADO");
            al_draw_text(fonte_hud, al_map_rgb(255, 255, 255),
                         LARGURA / 2.0, ALTURA / 2.0, ALLEGRO_ALIGN_CENTER,
                         "Pressione ESC para continuar");
        }
        if (roda_aberta)
            desenhar_roda_habilidade(sam_spr.attack1, sam_spr.attack2, sam_spr.attack3,
                                     habilidade1, habilidade2, habilidade3,
                                     roda_selecao, fonte_hud);

        al_flip_display();

        /* ?? game over / vitória ?? */
        morte_pronta = (jogador.estado == SAM_DEAD && !jogador.morte_animando);
        if (morte_pronta || sanidade.game_over)
        {
            if (!som_gameover_tocado)
            {
                parar_musica_fundo(&sons);
                tocar(sons.game_over_som);
                som_gameover_tocado = 1;
            }
            al_rest(0.8);
            const char *motivo = sanidade.game_over
                                     ? "Sua sanidade chegou ao limite..."
                                     : "Voce foi derrotado pelos zumbis!";
            int reiniciar = tela_game_over(queue, timer, fonte, fonte_hud, motivo);
            if (reiniciar)
            {
                jogador = (Jogador){0};
                jogador.mov.x = 60;
                jogador.mov.y = 253;
                jogador.tipo_ataque = 1;
                jogador.estamina = MAX_ESTAMINA;
                jogador.estado = SAM_IDLE;
                jogador.hurt_inicio = -999.0;
                jogador.ultimo_ataque = -999.0;
                jogador.dash_fuga_ultimo = -999.0;

                sanidade = (Sanidade){MAX_SANIDADE, 0, 0, 0, 0.0, 0};
                sanidade.ultimo_abate = al_get_time();

                horda_init(&horda);

                /* zera projéteis e explosões ácidas */
                for (int ip = 0; ip < MAX_PROJETEIS_ACIDO; ip++)
                    g_projeteis_acido[ip].ativo = 0;
                for (int ie = 0; ie < MAX_EXPLOSOES_ACIDAS; ie++)
                    g_explosoes_acidas[ie].ativo = 0;

                pocao = (Pocao){0};
                pocao_ultimo_check = 0.0;

                for (int i = 0; i < MAX_VIDAS; i++)
                {
                    vetor_vidas[i].ativa = 1;
                    strcpy(vetor_vidas[i].status, "Inteira");
                }

                tempo = (Temporizador){0};
                tempo.inicio = al_get_time();
                tempo.ativo = 1;
                carregar_ranking(&tempo);

                pausado = 0;
                roda_aberta = 0;
                roda_selecao = 1;
                esc_ant = 0;
                k_ant = 0;
                j_ant = 0;
                q_ant = 0;
                e_ant = 0;
                som_finish_tocado = 0;
                som_gameover_tocado = 0;

                tocar_musica_fundo(&sons);
                al_flush_event_queue(queue);
                continue;
            }
            rodando = 0;
            continue;
        }

        if (horda.fase_concluida && !sanidade.game_over)
        {
            if (!som_finish_tocado)
            {
                parar_musica_fundo(&sons);
                tocar(sons.finish_som);
                som_finish_tocado = 1;
            }
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA, al_map_rgba(0, 120, 40, 30));
            al_draw_text(fonte, al_map_rgb(255, 215, 0),
                         LARGURA / 2.0f, ALTURA / 2.0f - 60, ALLEGRO_ALIGN_CENTER,
                         "50 ZUMBIS ELIMINADOS!");
            al_draw_text(fonte_hud, al_map_rgb(255, 255, 255),
                         LARGURA / 2.0f, ALTURA / 2.0f, ALLEGRO_ALIGN_CENTER,
                         "Abrindo ranking...");
            al_flip_display();
            al_rest(2.0);
            tela_ranking(queue, timer, fonte, fonte_hud, &tempo, tempo.atual);
            rodando = 0;
            continue;
        }
    } /* fim while(rodando) */

    /* ?? limpeza ?? */
    free(vetor_vidas);
    liberar_matriz(matriz_dec, lm * cm);
    destruir_sons(&sons);
    al_destroy_bitmap(bg_menu);
    al_destroy_bitmap(bg);
    al_destroy_bitmap(mapa);
    al_destroy_bitmap(sam_spr.idle);
    al_destroy_bitmap(sam_spr.run);
    al_destroy_bitmap(sam_spr.jump);
    al_destroy_bitmap(sam_spr.attack1);
    al_destroy_bitmap(sam_spr.attack2);
    al_destroy_bitmap(sam_spr.attack3);
    al_destroy_bitmap(sam_spr.hurt);
    al_destroy_bitmap(sam_spr.dead);
    al_destroy_bitmap(zum_spr.walk);
    al_destroy_bitmap(zum_spr.run);
    al_destroy_bitmap(zum_spr.attack1);
    al_destroy_bitmap(zum_spr.attack2);
    al_destroy_bitmap(zum_spr.attack3);
    al_destroy_bitmap(zum_spr.bite);
    al_destroy_bitmap(zum_spr.hurt);
    al_destroy_bitmap(zum_spr.dead);
    al_destroy_bitmap(zum_spr.idle);
    al_destroy_bitmap(zum_acido_spr.walk);
    al_destroy_bitmap(zum_acido_spr.attack);
    al_destroy_bitmap(zum_acido_spr.hurt);
    al_destroy_bitmap(zum_acido_spr.dead);
    al_destroy_bitmap(zum_acido_spr.idle);
    al_destroy_bitmap(spr_acido_projetil);
    for (int i = 0; i < FRAMES_EXPLOSAO_ACIDA; i++)
        if (explosao_acida_frames[i])
            al_destroy_bitmap(explosao_acida_frames[i]);
    al_destroy_bitmap(coracao);
    al_destroy_bitmap(spr_estamina);
    if (spr_sanidade)
        al_destroy_bitmap(spr_sanidade);
    if (spr_pocao)
        al_destroy_bitmap(spr_pocao);
    al_destroy_bitmap(habilidade1);
    al_destroy_bitmap(habilidade2);
    al_destroy_bitmap(habilidade3);
    al_destroy_font(fonte_hud);
    al_destroy_font(fonte);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}