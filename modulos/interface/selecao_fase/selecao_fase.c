/* ================================================================== */
/* selecao_fase.c                                                     */
/* Tela de sele&#65533;&#65533;o de fases + progresso persistente                   */
/* ================================================================== */
#include "selecao_fase.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../mapa/mapa.h"
#include "../../sistemas/sons/sons.h"

/* ================================================================== */
/* Dimens&#65533;es/layout                                                   */
/* ================================================================== */
#ifndef LARGURA
#define LARGURA 1280
#endif
#ifndef ALTURA
#define ALTURA 720
#endif

/* Bot&#65533;es de fase */
#define BTN_W 200
#define BTN_H 60
#define BTN_PAD_X 8
#define BTN_PAD_Y 10
#define COLS 5

/* Bot&#65533;o "Voltar" centralizado na parte inferior */
#define VOLTAR_W 160
#define VOLTAR_H 50
#define VOLTAR_X ((LARGURA - VOLTAR_W) / 2)
#define VOLTAR_Y (ALTURA - 90)

/* Painel central ? usa quase toda a tela */
#define PAINEL_MARGEM_H 40
#define PAINEL_MARGEM_V 30
#define PAINEL_X PAINEL_MARGEM_H
#define PAINEL_Y PAINEL_MARGEM_V
#define PAINEL_W (LARGURA - 2 * PAINEL_MARGEM_H)
#define PAINEL_H (ALTURA - 2 * PAINEL_MARGEM_V - 80)
#define PAINEL_R 18

/* ================================================================== */
/* Box-blur por software (2 passes: horizontal + vertical)            */
/* ================================================================== */
static void aplicar_blur(ALLEGRO_BITMAP *bmp, int raio)
{
    if (!bmp || raio < 1)
        return;

    int w = al_get_bitmap_width(bmp);
    int h = al_get_bitmap_height(bmp);

    int flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *tmp = al_create_bitmap(w, h);
    al_set_new_bitmap_flags(flags);
    if (!tmp)
        return;

    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();

    /* Passe horizontal */
    al_set_target_bitmap(tmp);
    al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
    al_lock_bitmap(tmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float r = 0, g = 0, b = 0, a = 0;
            int cnt = 0;
            for (int k = -raio; k <= raio; k++)
            {
                int sx = x + k;
                if (sx < 0) sx = 0;
                if (sx >= w) sx = w - 1;
                ALLEGRO_COLOR c = al_get_pixel(bmp, sx, y);
                float cr, cg, cb, ca;
                al_unmap_rgba_f(c, &cr, &cg, &cb, &ca);
                r += cr; g += cg; b += cb; a += ca;
                cnt++;
            }
            al_put_pixel(x, y, al_map_rgba_f(r / cnt, g / cnt, b / cnt, a / cnt));
        }
    }
    al_unlock_bitmap(tmp);
    al_unlock_bitmap(bmp);

    /* Passe vertical */
    al_set_target_bitmap(bmp);
    al_lock_bitmap(tmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
    al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float r = 0, g = 0, b = 0, a = 0;
            int cnt = 0;
            for (int k = -raio; k <= raio; k++)
            {
                int sy = y + k;
                if (sy < 0) sy = 0;
                if (sy >= h) sy = h - 1;
                ALLEGRO_COLOR c = al_get_pixel(tmp, x, sy);
                float cr, cg, cb, ca;
                al_unmap_rgba_f(c, &cr, &cg, &cb, &ca);
                r += cr; g += cg; b += cb; a += ca;
                cnt++;
            }
            al_put_pixel(x, y, al_map_rgba_f(r / cnt, g / cnt, b / cnt, a / cnt));
        }
    }
    al_unlock_bitmap(bmp);
    al_unlock_bitmap(tmp);

    al_set_target_bitmap(old_target);
    al_destroy_bitmap(tmp);
}

/* ================================================================== */
/* Carrega e escala fundo com blur                                    */
/* ================================================================== */
static ALLEGRO_BITMAP *carregar_fundo_borrado(const char *caminho, int raio_blur)
{
    ALLEGRO_BITMAP *orig = al_load_bitmap(caminho);
    if (!orig)
    {
        fprintf(stderr, "Erro ao carregar: %s\n", caminho);
        return NULL;
    }

    int flags_antigos = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *scaled = al_create_bitmap(LARGURA, ALTURA);
    al_set_new_bitmap_flags(flags_antigos);

    if (!scaled)
    {
        al_destroy_bitmap(orig);
        return NULL;
    }

    ALLEGRO_BITMAP *old = al_get_target_bitmap();
    al_set_target_bitmap(scaled);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(orig, 0, 0,
                          al_get_bitmap_width(orig), al_get_bitmap_height(orig),
                          0, 0, LARGURA, ALTURA, 0);
    al_set_target_bitmap(old);
    al_destroy_bitmap(orig);

    if (raio_blur > 0)
        aplicar_blur(scaled, raio_blur);
    return scaled;
}

/* ================================================================== */
/* Progresso persistente                                              */
/* ================================================================== */
void progresso_carregar(ProgressoJogo *prog)
{
    prog->fases_desbloqueadas = 0;
    prog->ultima_fase_jogada = 0;

    FILE *f = fopen(PROGRESSO_ARQUIVO, "rb");
    if (!f)
        return;
    if (fread(prog, sizeof(ProgressoJogo), 1, f) != 1)
    {
        memset(prog, 0, sizeof(ProgressoJogo));
    }
    fclose(f);
}

void progresso_avancar_fase(ProgressoJogo *prog, int fase_idx)
{
    if (fase_idx > prog->ultima_fase_jogada)
        prog->ultima_fase_jogada = fase_idx;
    if (fase_idx + 1 > prog->fases_desbloqueadas)
        prog->fases_desbloqueadas = fase_idx + 1;

    FILE *f = fopen(PROGRESSO_ARQUIVO, "wb");
    if (!f)
    {
        system("mkdir -p saves");
        f = fopen(PROGRESSO_ARQUIVO, "wb");
    }
    if (f)
    {
        fwrite(prog, sizeof(ProgressoJogo), 1, f);
        fclose(f);
    }
}

/* ================================================================== */
/* Helpers de desenho                                                 */
/* ================================================================== */
static void desenhar_painel(float x, float y, float w, float h, float r)
{
    al_draw_filled_rounded_rectangle(x + 6, y + 8, x + w + 6, y + h + 8,
                                     r, r, al_map_rgba(0, 0, 0, 80));
    al_draw_filled_rounded_rectangle(x, y, x + w, y + h,
                                     r, r, al_map_rgba(8, 14, 22, 200));
    al_draw_rounded_rectangle(x, y, x + w, y + h,
                              r, r,
                              al_map_rgba(80, 140, 80, 80),
                              1.5f);
    al_draw_line(x + r, y + 1, x + w - r, y + 1,
                 al_map_rgba(160, 220, 120, 60), 1.0f);
}

static void desenhar_botao(float x, float y, float w, float h,
                           const char *label,
                           ALLEGRO_FONT *fonte,
                           int hover,
                           int bloqueado,
                           int destaque)
{
    ALLEGRO_COLOR cor_fundo, cor_borda, cor_texto, cor_glow;

    if (bloqueado)
    {
        cor_fundo = al_map_rgba(20, 22, 28, 180);
        cor_borda = al_map_rgba(50, 55, 65, 160);
        cor_texto = al_map_rgba(70, 75, 85, 200);
        cor_glow  = al_map_rgba(0, 0, 0, 0);
    }
    else if (hover)
    {
        cor_fundo = al_map_rgba(55, 130, 55, 230);
        cor_borda = al_map_rgba(160, 230, 90, 255);
        cor_texto = al_map_rgb(255, 255, 230);
        cor_glow  = al_map_rgba(100, 220, 80, 50);
    }
    else if (destaque)
    {
        cor_fundo = al_map_rgba(40, 100, 40, 210);
        cor_borda = al_map_rgba(130, 210, 70, 220);
        cor_texto = al_map_rgb(230, 255, 180);
        cor_glow  = al_map_rgba(80, 180, 60, 30);
    }
    else
    {
        cor_fundo = al_map_rgba(22, 50, 22, 200);
        cor_borda = al_map_rgba(70, 140, 50, 180);
        cor_texto = al_map_rgb(190, 215, 140);
        cor_glow  = al_map_rgba(0, 0, 0, 0);
    }

    if (hover || destaque)
        al_draw_filled_rounded_rectangle(x - 3, y - 3, x + w + 3, y + h + 3,
                                         11, 11, cor_glow);

    al_draw_filled_rounded_rectangle(x + 3, y + 4, x + w + 3, y + h + 4,
                                     9, 9, al_map_rgba(0, 0, 0, 60));
    al_draw_filled_rounded_rectangle(x, y, x + w, y + h, 9, 9, cor_fundo);

    if (!bloqueado)
        al_draw_filled_rounded_rectangle(x + 2, y + 2, x + w - 2, y + h * 0.45f,
                                         7, 7, al_map_rgba(255, 255, 255, 12));

    al_draw_rounded_rectangle(x, y, x + w, y + h, 9, 9, cor_borda, 1.8f);

    if (bloqueado)
    {
        float cx = x + 22, cy = y + h / 2.0f;
        al_draw_filled_circle(cx, cy, 7, al_map_rgba(60, 65, 75, 200));
        al_draw_filled_rectangle(cx - 5, cy, cx + 5, cy + 7, al_map_rgba(60, 65, 75, 200));
        al_draw_circle(cx, cy, 4, al_map_rgba(90, 95, 110, 220), 1.5f);
    }

    int lh = al_get_font_line_height(fonte);
    al_draw_text(fonte, cor_texto,
                 x + w / 2.0f, y + (h - lh) / 2.0f,
                 ALLEGRO_ALIGN_CENTER, label);
}

static int ponto_em_rect(float px, float py,
                         float rx, float ry, float rw, float rh)
{
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

static void desenhar_barra_progresso(float x, float y, float w, float h,
                                     float pct, ALLEGRO_COLOR cor_fill)
{
    al_draw_filled_rounded_rectangle(x, y, x + w, y + h,
                                     h / 2.0f, h / 2.0f, al_map_rgba(20, 30, 20, 180));
    al_draw_rounded_rectangle(x, y, x + w, y + h,
                              h / 2.0f, h / 2.0f, al_map_rgba(80, 120, 60, 140), 1.2f);
    if (pct > 0.01f)
    {
        float fw = (w - 4) * pct;
        if (fw < h) fw = h;
        al_draw_filled_rounded_rectangle(x + 2, y + 2, x + 2 + fw, y + h - 2,
                                         (h - 4) / 2.0f, (h - 4) / 2.0f, cor_fill);
        al_draw_filled_rounded_rectangle(x + 2, y + 2, x + 2 + fw, y + h / 2.0f,
                                         (h - 4) / 2.0f, (h - 4) / 2.0f, al_map_rgba(255, 255, 255, 30));
    }
}

/* ================================================================== */
/* Fundo pr&#65533;prio da tela de sele&#65533;&#65533;o                                   */
/* ================================================================== */
static ALLEGRO_BITMAP *criar_fundo_selecao(void)
{
    int flags_antigos = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *bmp = al_create_bitmap(LARGURA, ALTURA);
    al_set_new_bitmap_flags(flags_antigos);
    if (!bmp)
        return NULL;

    ALLEGRO_BITMAP *old = al_get_target_bitmap();
    al_set_target_bitmap(bmp);

    int faixas = 64;
    for (int i = 0; i < faixas; i++)
    {
        float t = (float)i / (float)(faixas - 1);
        unsigned char r = (unsigned char)(16 + (4  - 16) * t);
        unsigned char g = (unsigned char)(30 + (8  - 30) * t);
        unsigned char b = (unsigned char)(24 + (9  - 24) * t);
        float y0 = (ALTURA / (float)faixas) * i;
        float y1 = (ALTURA / (float)faixas) * (i + 1) + 1.5f;
        al_draw_filled_rectangle(0, y0, LARGURA, y1, al_map_rgb(r, g, b));
    }

    for (int i = 0; i < 50; i++)
    {
        float x = (float)(rand() % LARGURA);
        float y = (float)(rand() % ALTURA);
        float raio = 1.0f + (float)(rand() % 3);
        int alfa = 30 + rand() % 70;
        al_draw_filled_circle(x, y, raio, al_map_rgba(150, 220, 140, alfa));
    }

    al_draw_filled_rectangle(0, 0, LARGURA, 100, al_map_rgba(0, 0, 0, 100));
    al_draw_filled_rectangle(0, ALTURA - 100, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 100));
    al_draw_filled_rectangle(0, 0, 160, ALTURA, al_map_rgba(0, 0, 0, 70));
    al_draw_filled_rectangle(LARGURA - 160, 0, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 70));

    al_set_target_bitmap(old);
    return bmp;
}

/* ================================================================== */
/* tela_selecao_fase                                                  */
/* ================================================================== */
#define CURSOR_VOLTAR total   /* sentinel: cursor aponta pro bot&#65533;o Voltar */

int tela_selecao_fase(ALLEGRO_EVENT_QUEUE *queue,
                      ALLEGRO_TIMER *timer,
                      ALLEGRO_FONT *fonte,
                      ALLEGRO_FONT *fonte_hud,
                      ProgressoJogo *prog,
                      Sons *sons)
{
    ALLEGRO_BITMAP *fundo = criar_fundo_selecao();

    int total = mapa_total();
    int escolhida = -2;

    float mouse_x = 0.0f, mouse_y = 0.0f;

    int total_cols = (total < COLS) ? total : COLS;
    int total_rows = (total + COLS - 1) / COLS;

    float grade_w = total_cols * (BTN_W + BTN_PAD_X) - BTN_PAD_X;
    float grade_h = total_rows * (BTN_H + BTN_PAD_Y) - BTN_PAD_Y;

    /* Alturas fixas dos elementos de cabe&#65533;alho */
    float bloco_titulo_h = 32.0f;
    float bloco_sub_h    = 18.0f;
    float bloco_gap1     = 16.0f;  /* titulo -> subtitulo  */
    float bloco_gap2     = 32.0f;  /* subtitulo -> botoes  */
    float bloco_total_h  = bloco_titulo_h + bloco_gap1
                           + bloco_sub_h  + bloco_gap2 + grade_h;

    /* Centraliza o bloco verticalmente (reserva 70px para o Voltar) */
    float bloco_y  = (ALTURA - bloco_total_h - 70.0f) / 2.0f;

    /* Grade centralizada horizontalmente */
    float origin_x = (LARGURA - grade_w) / 2.0f;
    float origin_y = bloco_y + bloco_titulo_h + bloco_gap1 + bloco_sub_h + bloco_gap2;

    /* Coordenadas dos textos */
    float titulo_cx  = LARGURA / 2.0f;
    float titulo_top = bloco_y;
    float sub_top    = bloco_y + bloco_titulo_h + bloco_gap1;

    /* Painel envolvendo tudo com margem interna.
     * pad_h maior que pad_v para acomodar o subt&#65533;tulo que &#65533; mais largo
     * que a grade de bot&#65533;es (ex: "A / D  ou  Setas para navegar | Enter"). */
    float painel_pad_v = 36.0f;
    float painel_pad_h = 60.0f;   /* horizontal extra para o texto caber */
    float painel_x   = origin_x - painel_pad_h;
    float painel_y   = titulo_top - painel_pad_v;
    float painel_w   = grade_w + painel_pad_h * 2.0f;
    float painel_h   = bloco_total_h + painel_pad_v * 2.0f;

    int cursor = prog->ultima_fase_jogada;
    if (cursor > prog->fases_desbloqueadas) cursor = prog->fases_desbloqueadas;
    if (cursor < 0) cursor = 0;

    /* ---------------------------------------------------------------- */
    /* Loop principal                                                   */
    /* ---------------------------------------------------------------- */
    while (escolhida == -2)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            escolhida = -1;
            break;
        }

        /* ---- mouse ------------------------------------------------- */
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            mouse_x = (float)ev.mouse.x;
            mouse_y = (float)ev.mouse.y;
        }

        /* ---- teclado ----------------------------------------------- */
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            int kc = ev.keyboard.keycode;

            if (kc == ALLEGRO_KEY_ESCAPE)
            {
                tocar(sons->esc_som);
                escolhida = -1;
                break;
            }

            /* --- seta/A: mover para a esquerda ----------------------- */
            if (kc == ALLEGRO_KEY_A || kc == ALLEGRO_KEY_LEFT)
            {
                if (cursor == CURSOR_VOLTAR)
                {
                    /* Do bot&#65533;o Voltar volta para a &#65533;ltima fase dispon&#65533;vel */
                    int novo = prog->fases_desbloqueadas;
                    if (novo >= total) novo = total - 1;
                    cursor = novo;
                    tocar(sons->selecao_som);
                }
                else if (cursor > 0)
                {
                    cursor--;
                    tocar(sons->selecao_som);
                }
            }
            /* --- seta/D: mover para a direita ------------------------ */
            else if (kc == ALLEGRO_KEY_D || kc == ALLEGRO_KEY_RIGHT)
            {
                if (cursor < CURSOR_VOLTAR)
                {
                    int novo = cursor + 1;
                    if (novo <= prog->fases_desbloqueadas && novo < total)
                    {
                        cursor = novo;
                        tocar(sons->selecao_som);
                    }
                    else
                    {
                        /* Chegou no limite: vai para Voltar */
                        cursor = CURSOR_VOLTAR;
                        tocar(sons->selecao_som);
                    }
                }
            }
            /* --- seta cima / W ------------------------------------------ */
            else if (kc == ALLEGRO_KEY_UP || kc == ALLEGRO_KEY_W)
            {
                if (cursor == CURSOR_VOLTAR)
                {
                    /* De Voltar sobe para a &#65533;ltima linha de fases */
                    int ultima_linha_inicio = ((total - 1) / COLS) * COLS;
                    int destino = ultima_linha_inicio + (prog->fases_desbloqueadas % COLS);
                    if (destino > prog->fases_desbloqueadas) destino = prog->fases_desbloqueadas;
                    if (destino >= total) destino = total - 1;
                    cursor = destino;
                    tocar(sons->selecao_som);
                }
                else
                {
                    int novo = cursor - COLS;
                    if (novo >= 0)
                    {
                        cursor = novo;
                        tocar(sons->selecao_som);
                    }
                }
            }
            /* --- seta baixo / S ----------------------------------------- */
            else if (kc == ALLEGRO_KEY_DOWN || kc == ALLEGRO_KEY_S)
            {
                if (cursor != CURSOR_VOLTAR)
                {
                    int novo = cursor + COLS;
                    if (novo < total && novo <= prog->fases_desbloqueadas)
                    {
                        cursor = novo;
                        tocar(sons->selecao_som);
                    }
                    else
                    {
                        cursor = CURSOR_VOLTAR;
                        tocar(sons->selecao_som);
                    }
                }
            }
            /* --- confirmar ------------------------------------------ */
            else if (kc == ALLEGRO_KEY_ENTER || kc == ALLEGRO_KEY_SPACE)
            {
                if (cursor == CURSOR_VOLTAR)
                {
                    tocar(sons->esc_som);
                    escolhida = -1;
                    break;
                }
                else
                {
                    tocar(sons->jogar);
                    prog->ultima_fase_jogada = cursor;
                    escolhida = cursor;
                    break;
                }
            }
        }

        /* ---- clique de mouse --------------------------------------- */
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
        {
            float mx = (float)ev.mouse.x, my = (float)ev.mouse.y;

            if (ponto_em_rect(mx, my,
                              (float)VOLTAR_X, (float)VOLTAR_Y,
                              (float)VOLTAR_W, (float)VOLTAR_H))
            {
                tocar(sons->esc_som);
                escolhida = -1;
                break;
            }

            for (int i = 0; i < total; i++)
            {
                if (i > prog->fases_desbloqueadas)
                    continue;

                int col = i % COLS, row = i / COLS;
                float bx = origin_x + col * (BTN_W + BTN_PAD_X);
                float by = origin_y + row * (BTN_H + BTN_PAD_Y);

                if (ponto_em_rect(mx, my, bx, by, BTN_W, BTN_H))
                {
                    tocar(sons->jogar);
                    prog->ultima_fase_jogada = i;
                    escolhida = i;
                    break;
                }
            }
        }

        /* ---- RENDERIZA&#65533;&#65533;O: s&#65533; no tick do timer; descarta outros ----- *
         * FIX LAG: drena todos os eventos pendentes antes de desenhar,  *
         * assim um burst de eventos de mouse n&#65533;o atrasa o frame.        */
        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;
        if (!al_is_event_queue_empty(queue))
            continue;   /* ainda h&#65533; eventos para processar; n&#65533;o perde o frame, *
                         * pois o timer vai disparar novamente no pr&#65533;ximo tick  */

        /* ---------- desenho ----------------------------------------- */
        if (fundo)
        {
            al_draw_bitmap(fundo, 0, 0, 0);
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgba(8, 14, 22, 160));
        }
        else
        {
            al_clear_to_color(al_map_rgb(8, 14, 22));
        }

        desenhar_painel(painel_x, painel_y, painel_w, painel_h, PAINEL_R);

        /* T&#65533;tulo centralizado */
        al_draw_text(fonte, al_map_rgb(210, 185, 60),
                     titulo_cx, titulo_top,
                     ALLEGRO_ALIGN_CENTER, "SELECIONAR FASE");

        al_draw_line(titulo_cx - 110.0f, sub_top - 4.0f,
                     titulo_cx + 110.0f, sub_top - 4.0f,
                     al_map_rgba(180, 150, 50, 120), 1.5f);

        al_draw_text(fonte_hud, al_map_rgba(170, 180, 160, 190),
                     titulo_cx, sub_top,
                     ALLEGRO_ALIGN_CENTER,
                     "Selecione a Fase para Jogar");

        /* ---------- bot&#65533;es de fase ---------------------------------- */
        for (int i = 0; i < total; i++)
        {
            int col = i % COLS, row = i / COLS;
            float bx = origin_x + col * (BTN_W + BTN_PAD_X);
            float by = origin_y + row * (BTN_H + BTN_PAD_Y);

            int bloqueado = (i > prog->fases_desbloqueadas);

            /* hover = cursor teclado OU mouse em cima (nunca bloqueado).
             * destaque REMOVIDO: era a causa do bot&#65533;o mais claro ?
             * ultima_fase_jogada ficava com estado visual diferente
             * dos demais desbloqueados sem o usu&#65533;rio querer.          */
            int hover = (!bloqueado &&
                         (cursor == i ||
                          ponto_em_rect(mouse_x, mouse_y, bx, by, BTN_W, BTN_H)));

            char label[32];
            snprintf(label, sizeof(label), "FASE %d", i + 1);

            desenhar_botao(bx, by, BTN_W, BTN_H, label,
                           fonte_hud, hover, bloqueado, 0);
        }

        /* ---------- barra de progresso ------------------------------ */
        {
            int total_f = (total > 0) ? total : 1;
            float pct   = (float)prog->fases_desbloqueadas / (float)total_f;

            float barra_w = grade_w;
            float barra_h = 10.0f;
            float barra_x = origin_x;
            float barra_y = origin_y + grade_h + 18.0f;

            desenhar_barra_progresso(barra_x, barra_y, barra_w, barra_h,
                                     pct, al_map_rgba(100, 210, 80, 220));

            char txt_prog[64];
            snprintf(txt_prog, sizeof(txt_prog),
                     "%d / %d fases concluidas",
                     prog->fases_desbloqueadas, total);
            al_draw_text(fonte_hud, al_map_rgba(150, 210, 130, 210),
                         titulo_cx, barra_y + barra_h + 8.0f,
                         ALLEGRO_ALIGN_CENTER, txt_prog);
        }

        /* ---------- bot&#65533;o Voltar ------------------------------------ */
        {
            /*
             * FIX COR bot&#65533;o Voltar: hover quando cursor==CURSOR_VOLTAR
             * (teclado) OU quando o mouse est&#65533; em cima.
             * Antes o `cursor == CURSOR_VOLTAR` estava embutido numa
             * express&#65533;o OR que usava a macro n&#65533;o-definida neste escopo.
             */
            int hover_v = (cursor == CURSOR_VOLTAR ||
                           ponto_em_rect(mouse_x, mouse_y,
                                         (float)VOLTAR_X, (float)VOLTAR_Y,
                                         (float)VOLTAR_W, (float)VOLTAR_H));
            desenhar_botao((float)VOLTAR_X, (float)VOLTAR_Y,
                           (float)VOLTAR_W, (float)VOLTAR_H,
                           "< VOLTAR", fonte_hud, hover_v, 0, 0);
        }

        al_flip_display();
    }

    if (fundo)
        al_destroy_bitmap(fundo);
    al_flush_event_queue(queue);
    return escolhida;
}

/* ================================================================== */
/* tela_proxima_fase                                                  */
/* ================================================================== */
void tela_proxima_fase(ALLEGRO_EVENT_QUEUE *queue,
                       ALLEGRO_TIMER *timer,
                       ALLEGRO_FONT *fonte,
                       ALLEGRO_FONT *fonte_hud,
                       int fase_atual,
                       int proximo_idx,
                       const void *cfg_prox_void)
{
    const ConfigMapa *cfg = (const ConfigMapa *)cfg_prox_void;

    ALLEGRO_BITMAP *fundo =
        carregar_fundo_borrado("assets/cenarios/inicio.png", 8);

    double inicio     = al_get_time();
    double duracao_max = 5.0;
    int    continuar  = 0;

    while (!continuar)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            int kc = ev.keyboard.keycode;
            if (kc == ALLEGRO_KEY_ENTER || kc == ALLEGRO_KEY_SPACE ||
                kc == ALLEGRO_KEY_ESCAPE)
                continuar = 1;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            continuar = 1;

        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;
        if (!al_is_event_queue_empty(queue))
            continue;

        double elapsed = al_get_time() - inicio;
        if (elapsed >= duracao_max)
            break;

        if (fundo)
        {
            al_draw_bitmap(fundo, 0, 0, 0);
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgba(5, 12, 8, 180));
        }
        else
        {
            al_clear_to_color(al_map_rgb(5, 12, 8));
        }

        float pw = 600.0f, ph = 340.0f;
        float px = (LARGURA - pw) / 2.0f, py = (ALTURA - ph) / 2.0f;
        desenhar_painel(px, py, pw, ph, 16);

        float cx = LARGURA / 2.0f;
        al_draw_text(fonte, al_map_rgb(255, 210, 50),
                     cx, py + 28.0f, ALLEGRO_ALIGN_CENTER,
                     "FASE CONCLUIDA");

        al_draw_line(cx - 200, py + 76.0f, cx + 200, py + 76.0f,
                     al_map_rgba(150, 200, 80, 100), 1.0f);

        char txt_prox[80];
        if (cfg && cfg->nome && cfg->nome[0] != '\0')
            snprintf(txt_prox, sizeof(txt_prox),
                     "Proxima: Fase %d - %s", proximo_idx + 1, cfg->nome);
        else
            snprintf(txt_prox, sizeof(txt_prox),
                     "Proxima: Fase %d", proximo_idx + 1);
        al_draw_text(fonte, al_map_rgb(200, 230, 140),
                     cx, py + 96.0f, ALLEGRO_ALIGN_CENTER, txt_prox);

        float pct     = 1.0f - (float)(elapsed / duracao_max);
        float barra_w = 380.0f, barra_h = 14.0f;
        float barra_x = cx - barra_w / 2.0f, barra_y = py + 180.0f;
        desenhar_barra_progresso(barra_x, barra_y, barra_w, barra_h,
                                 pct, al_map_rgba(100, 210, 80, 220));

        int restam = (int)(duracao_max - elapsed) + 1;
        char txt_cnt[32];
        snprintf(txt_cnt, sizeof(txt_cnt), "Iniciando em %d...", restam);
        al_draw_text(fonte_hud, al_map_rgba(190, 200, 180, 210),
                     cx, barra_y + barra_h + 14.0f,
                     ALLEGRO_ALIGN_CENTER, txt_cnt);

        al_draw_text(fonte_hud, al_map_rgba(140, 150, 130, 170),
                     cx, barra_y + barra_h + 42.0f,
                     ALLEGRO_ALIGN_CENTER,
                     "Pressione ENTER para continuar");

        al_flip_display();
    }

    if (fundo)
        al_destroy_bitmap(fundo);
    al_flush_event_queue(queue);
}
