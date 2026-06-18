#include "interface.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>

#include "../sistemas/sons/sons.h"
#include "../sistemas/vida/vida.h"

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

            /* ── botão JOGAR ── */
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

            /* ── botão SAIR ── */
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

    ALLEGRO_COLOR cor;
    if (vivas >= 4)
        cor = al_map_rgb(0, 200, 0);
    else if (vivas >= 2)
        cor = al_map_rgb(220, 200, 0);
    else
        cor = al_map_rgb(200, 0, 0);

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

    ALLEGRO_COLOR cor;
    if (estamina > 0)
        cor = al_map_rgb(30, 144, 255);
    else
        cor = al_map_rgb(80, 80, 80);

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

    ALLEGRO_COLOR cor;
    if (san->valor > 0)
        cor = al_map_rgb(180, g, b);
    else
        cor = al_map_rgb(80, 80, 80);

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

        if (sel)
            al_draw_filled_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6, al_map_rgb(218, 165, 32));
        else
            al_draw_filled_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6, al_map_rgb(60, 35, 10));

        if (sel)
            al_draw_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6, al_map_rgb(218, 165, 32), 3);
        else
            al_draw_rounded_rectangle(sx, sy, sx + tam, sy + tam, 6, 6, al_map_rgb(218, 165, 32), 1);

        float ip = -14.0f;
        al_draw_scaled_bitmap(sprites[i], 0, 0, al_get_bitmap_width(sprites[i]), al_get_bitmap_height(sprites[i]),
                              sx + ip, sy + ip, tam - ip * 1.6f, tam - ip * 1.6f, 0);

        if (sel)
            al_draw_text(fonte_hud, al_map_rgb(255, 215, 0), sx + tam / 2.0f, sy + tam + 4, ALLEGRO_ALIGN_CENTER, nomes[i]);
        else
            al_draw_text(fonte_hud, al_map_rgb(180, 180, 180), sx + tam / 2.0f, sy + tam + 4, ALLEGRO_ALIGN_CENTER, nomes[i]);

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

    float cd_pct;
    if (elapsed >= DASH_FUGA_DELAY)
        cd_pct = 1.0f;
    else
        cd_pct = (float)(elapsed / DASH_FUGA_DELAY);

    int pode = (jog->estamina >= DASH_FUGA_CUSTO) && cd_pct >= 1.0f;
    float bx = 1805.0f, by = 930.0f, bw = 90.0f, bh = 88.0f;
    al_draw_filled_rounded_rectangle(bx + 3, by + 3, bx + bw + 3, by + bh + 3, 6, 6, al_map_rgba(0, 0, 0, 120));

    if (pode)
        al_draw_filled_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6, al_map_rgb(0, 160, 220));
    else
        al_draw_filled_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6, al_map_rgb(30, 30, 60));

    if (pode)
        al_draw_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6, al_map_rgb(0, 220, 255), 3);
    else
        al_draw_rounded_rectangle(bx, by, bx + bw, by + bh, 6, 6, al_map_rgb(80, 80, 120), 1);

    if (cd_pct < 1.0f)
    {
        al_draw_filled_rectangle(bx + 4, by + bh - 10, bx + 4 + (bw - 8) * cd_pct, by + bh - 4, al_map_rgb(0, 200, 255));
        al_draw_rectangle(bx + 4, by + bh - 10, bx + bw - 4, by + bh - 4, al_map_rgb(0, 100, 150), 1);
    }

    if (pode)
        al_draw_text(fonte_hud, al_map_rgb(255, 255, 255), bx + bw / 2.0f, by + 22.0f, ALLEGRO_ALIGN_CENTER, "DASH");
    else
        al_draw_text(fonte_hud, al_map_rgb(100, 100, 140), bx + bw / 2.0f, by + 22.0f, ALLEGRO_ALIGN_CENTER, "DASH");

    if (pode)
        al_draw_text(fonte_hud, al_map_rgb(0, 255, 255), bx + bw / 2.0f, by + 42.0f, ALLEGRO_ALIGN_CENTER, "[E]");
    else
        al_draw_text(fonte_hud, al_map_rgb(80, 80, 120), bx + bw / 2.0f, by + 42.0f, ALLEGRO_ALIGN_CENTER, "[E]");
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

        ALLEGRO_COLOR cf;
        if (sel)
            cf = al_map_rgba(218, 165, 32, 210);
        else
            cf = al_map_rgba(60, 35, 10, 185);

        al_draw_filled_pieslice(cx, cy, r_ext, a0, step, cf);

        if (sel)
            al_draw_pieslice(cx, cy, r_ext, a0, step, al_map_rgb(218, 165, 32), 4);
        else
            al_draw_pieslice(cx, cy, r_ext, a0, step, al_map_rgb(218, 165, 32), 2);

        float dist = r_int + (r_ext - r_int) * 0.55f;
        float ix = cx + cosf(amid) * dist, iy = cy + sinf(amid) * dist;

        float itam;
        if (sel)
            itam = 200.0f;
        else
            itam = 150.0f;

        al_draw_scaled_bitmap(sprites[i], 0, 0, al_get_bitmap_width(sprites[i]), al_get_bitmap_height(sprites[i]),
                              ix - itam / 2.0f, iy - itam / 2.0f, itam, itam, 0);
        float tx = cx + cosf(amid) * (r_ext + 32.0f), ty = cy + sinf(amid) * (r_ext + 32.0f);

        if (sel)
            al_draw_text(fonte_hud, al_map_rgb(255, 215, 0), tx, ty - 14, ALLEGRO_ALIGN_CENTER, nomes[i]);
        else
            al_draw_text(fonte_hud, al_map_rgb(200, 200, 200), tx, ty - 14, ALLEGRO_ALIGN_CENTER, nomes[i]);
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