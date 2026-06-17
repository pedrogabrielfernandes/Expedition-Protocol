/* ================================================================== */
/*  main.c  –  Samurai vs Zumbis                                       */
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

#include "tipos.h"

/* ================================================================== */
/*  Globais de colisão                                                  */
/* ================================================================== */
unsigned char colisao[ALTURA][3000];
static ALLEGRO_BITMAP *jog_mapa_ptr = NULL;

/* Globais de projéteis e explosões de ácido */
static ProjetilAcido g_projeteis_acido[MAX_PROJETEIS_ACIDO];
static ExplosaoAcida g_explosoes_acidas[MAX_EXPLOSOES_ACIDAS];

/* ================================================================== */
/*  SISTEMA DE SONS                                                     */
/* ================================================================== */
static void tocar(ALLEGRO_SAMPLE *s)
{
    if (s)
        al_play_sample(s, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
}

static void tocar_pitch(ALLEGRO_SAMPLE *s, float pitch)
{
    if (s)
        al_play_sample(s, 1.0f, 0.0f, pitch, ALLEGRO_PLAYMODE_ONCE, NULL);
}

static void tocar_musica_fundo(Sons *s)
{
    if (!s->padrao || s->padrao_tocando)
        return;
    if (al_play_sample(s->padrao, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, &s->padrao_id))
        s->padrao_tocando = 1;
}

static void parar_musica_fundo(Sons *s)
{
    if (s->padrao_tocando)
    {
        al_stop_sample(&s->padrao_id);
        s->padrao_tocando = 0;
    }
}

Sons carregar_sons(void)
{
    Sons s;
    memset(&s, 0, sizeof(s));

    s.katana12 = al_load_sample("assets/sons/Samurai/Katana12.wav");
    s.katana_attack3 = al_load_sample("assets/sons/Samurai/Katana_attack3.wav");
    s.dash = al_load_sample("assets/sons/Samurai/dash.wav");
    s.acerto_zumbi = al_load_sample("assets/sons/Samurai/acerto_zumbi.wav");
    s.dano_sofrido = al_load_sample("assets/sons/Samurai/dano_sofrido.wav");
    s.pulo = al_load_sample("assets/sons/Samurai/pulo.wav");
    s.walk_sam = al_load_sample("assets/sons/Samurai/walk.wav");
    s.morrendo = al_load_sample("assets/sons/Samurai/morrendo.wav");
    s.caindo = al_load_sample("assets/sons/Samurai/caindo.wav");
    s.dash_fuga = s.dash;

    s.dano = al_load_sample("assets/sons/Zumbi/dano.wav");
    s.dano_miss = al_load_sample("assets/sons/Zumbi/dano_miss.wav");
    s.morte_zumbi = al_load_sample("assets/sons/Zumbi/morte_zumbi.wav");
    s.dano_zumbi = al_load_sample("assets/sons/Zumbi/dano_zumbi.wav");
    s.zumbi_knockback = al_load_sample("assets/sons/Zumbi/zumbi_KnockBack.ogg");
    s.som_velocista = al_load_sample("assets/sons/Zumbi/som_velocista.wav");
    s.ataque_velocista = al_load_sample("assets/sons/Zumbi/ataque_velocista.wav");
    s.dano_velocista = al_load_sample("assets/sons/Zumbi/dano_velocista.wav");
    s.morte_velocista = al_load_sample("assets/sons/Zumbi/morte_velocista.wav");
    s.dano_sofrido_velocista = al_load_sample("assets/sons/Zumbi/dano_sofrido_velocista.wav");

    s.padrao = al_load_sample("assets/sons/game/padrao.wav");
    s.jogar = al_load_sample("assets/sons/game/jogar.wav");
    s.esc_som = al_load_sample("assets/sons/game/esc.wav");
    s.game_over_som = al_load_sample("assets/sons/game/game_over.wav");
    s.pocao_som = al_load_sample("assets/sons/game/pocao.wav");
    s.sair_som = al_load_sample("assets/sons/game/sair.wav");
    s.finish_som = al_load_sample("assets/sons/game/finish.wav");
    s.selecao_som = al_load_sample("assets/sons/game/selecao.wav");
    s.horda_som = al_load_sample("assets/sons/Zumbi/horda.wav");

    s.acido_impacto = al_load_sample("assets/sons/Zumbi/acido/acido_impacto.wav");
    s.cuspindo_acido = al_load_sample("assets/sons/Zumbi/acido/cuspindo_acido.wav");
    s.dano_zumbi_acido = al_load_sample("assets/sons/Zumbi/acido/dano_zumbi_acido.wav");
    s.explosao_acida = al_load_sample("assets/sons/Zumbi/acido/explosao.wav");
    s.explosao_acida2 = al_load_sample("assets/sons/Zumbi/acido/explosao2.wav");
    s.morte_acido = al_load_sample("assets/sons/Zumbi/acido/morte_acido.wav");
    s.impacto_espada = al_load_sample("assets/sons/Zumbi/acido/impacto_espada.wav");

    s.padrao_tocando = 0;

    if (!s.katana12)
        puts("Aviso: Katana12.wav nao carregado");
    if (!s.katana_attack3)
        puts("Aviso: Katana_attack3.wav nao carregado");
    if (!s.dash)
        puts("Aviso: dash.wav nao carregado");
    if (!s.acerto_zumbi)
        puts("Aviso: acerto_zumbi.wav nao carregado");
    if (!s.dano_sofrido)
        puts("Aviso: dano_sofrido.wav nao carregado");
    if (!s.pulo)
        puts("Aviso: pulo.wav nao carregado");
    if (!s.walk_sam)
        puts("Aviso: Samurai/walk.wav nao carregado");
    if (!s.morrendo)
        puts("Aviso: morrendo.wav nao carregado");
    if (!s.caindo)
        puts("Aviso: caindo.wav nao carregado");
    if (!s.dano)
        puts("Aviso: dano.wav nao carregado");
    if (!s.dano_miss)
        puts("Aviso: dano_miss.wav nao carregado");
    if (!s.morte_zumbi)
        puts("Aviso: morte_zumbi.wav nao carregado");
    if (!s.dano_zumbi)
        puts("Aviso: dano_zumbi.wav nao carregado");
    if (!s.zumbi_knockback)
        puts("Aviso: zumbi_KnockBack.ogg nao carregado");
    if (!s.som_velocista)
        puts("Aviso: som_velocista.wav nao carregado");
    if (!s.ataque_velocista)
        puts("Aviso: ataque_velocista.wav nao carregado");
    if (!s.dano_velocista)
        puts("Aviso: dano_velocista.wav nao carregado");
    if (!s.morte_velocista)
        puts("Aviso: morte_velocista.wav nao carregado");
    if (!s.dano_sofrido_velocista)
        puts("Aviso: dano_sofrido_velocista.wav nao carregado");
    if (!s.padrao)
        puts("Aviso: padrao.wav nao carregado");
    if (!s.jogar)
        puts("Aviso: jogar.wav nao carregado");
    if (!s.esc_som)
        puts("Aviso: esc.wav nao carregado");
    if (!s.game_over_som)
        puts("Aviso: game_over.wav nao carregado");
    if (!s.pocao_som)
        puts("Aviso: pocao.wav nao carregado");
    if (!s.acido_impacto)
        puts("Aviso: acido_impacto.wav nao carregado");
    if (!s.cuspindo_acido)
        puts("Aviso: cuspindo_acido.wav nao carregado");
    if (!s.dano_zumbi_acido)
        puts("Aviso: dano_zumbi_acido.wav nao carregado");
    if (!s.explosao_acida)
        puts("Aviso: explosao.wav nao carregado");
    if (!s.explosao_acida2)
        puts("Aviso: explosao2.wav nao carregado");
    if (!s.morte_acido)
        puts("Aviso: morte_acido.wav nao carregado");
    if (!s.impacto_espada)
        puts("Aviso: impacto_espada.wav nao carregado");
    if (!s.sair_som)
        puts("Aviso: sair.wav nao carregado");
    if (!s.finish_som)
        puts("Aviso: finish.wav nao carregado");
    if (!s.selecao_som)
        puts("Aviso: selecao.wav nao carregado");
    if (!s.horda_som)
        puts("Aviso: horda.wav nao carregado");

    return s;
}

void destruir_sons(Sons *s)
{
    parar_musica_fundo(s);
    if (s->katana12)
        al_destroy_sample(s->katana12);
    if (s->katana_attack3)
        al_destroy_sample(s->katana_attack3);
    if (s->dash)
        al_destroy_sample(s->dash);
    if (s->acerto_zumbi)
        al_destroy_sample(s->acerto_zumbi);
    if (s->dano_sofrido)
        al_destroy_sample(s->dano_sofrido);
    if (s->pulo)
        al_destroy_sample(s->pulo);
    if (s->walk_sam)
        al_destroy_sample(s->walk_sam);
    if (s->morrendo)
        al_destroy_sample(s->morrendo);
    if (s->caindo)
        al_destroy_sample(s->caindo);
    if (s->dano)
        al_destroy_sample(s->dano);
    if (s->dano_miss)
        al_destroy_sample(s->dano_miss);
    if (s->morte_zumbi)
        al_destroy_sample(s->morte_zumbi);
    if (s->dano_zumbi)
        al_destroy_sample(s->dano_zumbi);
    if (s->zumbi_knockback)
        al_destroy_sample(s->zumbi_knockback);
    if (s->som_velocista)
        al_destroy_sample(s->som_velocista);
    if (s->ataque_velocista)
        al_destroy_sample(s->ataque_velocista);
    if (s->dano_velocista)
        al_destroy_sample(s->dano_velocista);
    if (s->morte_velocista)
        al_destroy_sample(s->morte_velocista);
    if (s->dano_sofrido_velocista)
        al_destroy_sample(s->dano_sofrido_velocista);
    if (s->padrao)
        al_destroy_sample(s->padrao);
    if (s->jogar)
        al_destroy_sample(s->jogar);
    if (s->esc_som)
        al_destroy_sample(s->esc_som);
    if (s->game_over_som)
        al_destroy_sample(s->game_over_som);
    if (s->pocao_som)
        al_destroy_sample(s->pocao_som);
    if (s->acido_impacto)
        al_destroy_sample(s->acido_impacto);
    if (s->cuspindo_acido)
        al_destroy_sample(s->cuspindo_acido);
    if (s->dano_zumbi_acido)
        al_destroy_sample(s->dano_zumbi_acido);
    if (s->explosao_acida)
        al_destroy_sample(s->explosao_acida);
    if (s->explosao_acida2)
        al_destroy_sample(s->explosao_acida2);
    if (s->morte_acido)
        al_destroy_sample(s->morte_acido);
    if (s->impacto_espada)
        al_destroy_sample(s->impacto_espada);
    if (s->sair_som)
        al_destroy_sample(s->sair_som);
    if (s->finish_som)
        al_destroy_sample(s->finish_som);
    if (s->selecao_som)
        al_destroy_sample(s->selecao_som);
    if (s->horda_som)
        al_destroy_sample(s->horda_som);
}

/* ================================================================== */
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
void explosoes_acidas_atualizar(Jogador *jog, VidaStatus *vidas, Sons *sons);
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
/*  COLISÃO COM MAPA                                                    */
/* ================================================================== */
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
    int top = (int)y, bottom = (int)y + HITBOX_H - 1;
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
    int left = (int)x + 4, right = (int)x + HITBOX_W - 4, foot = (int)y + HITBOX_H;
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

/* ================================================================== */
/*  VIDAS                                                               */
/* ================================================================== */
int contar_vidas(VidaStatus *vidas)
{
    int c = 0;
    for (int i = 0; i < MAX_VIDAS; i++)
        if (vidas[i].ativa)
            c++;
    return c;
}

void restaurar_vidas(VidaStatus *vidas, int quantidade)
{
    int restauradas = 0;
    for (int i = 0; i < MAX_VIDAS && restauradas < quantidade; i++)
        if (!vidas[i].ativa)
        {
            vidas[i].ativa = 1;
            strcpy(vidas[i].status, "Inteira");
            restauradas++;
        }
}

void perder_vida(VidaStatus *vidas)
{
    for (int i = MAX_VIDAS - 1; i >= 0; i--)
        if (vidas[i].ativa)
        {
            vidas[i].ativa = 0;
            strcpy(vidas[i].status, "Perdida");
            break;
        }
}

/* ================================================================== */
/*  HUD – VIDAS / ESTAMINA / SANIDADE                                  */
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
/*  SANIDADE                                                            */
/* ================================================================== */
static int zumbis_no_ciclo(const Sanidade *san)
{
    return san->zumbis_mortos - san->ciclo_base;
}

void atualizar_sanidade(Sanidade *san)
{
    int no_ciclo = zumbis_no_ciclo(san);
    float alvo = MAX_SANIDADE - no_ciclo * QUEDA_POR_ZUMBI;
    if (alvo < 0.0f)
        alvo = 0.0f;

    double agora = al_get_time();
    double sem_matar = agora - san->ultimo_abate;

    if (san->valor > alvo)
        san->valor = alvo;
    if (san->valor <= 0.0f)
    {
        san->valor = 0.0f;
        san->game_over = 1;
        return;
    }
    if (sem_matar >= TEMPO_INICIO_RECUPERACAO && !san->game_over)
    {
        float dur_recup = (float)(TEMPO_RECUPERACAO_TOTAL - TEMPO_INICIO_RECUPERACAO);
        float t_recup = (float)(sem_matar - TEMPO_INICIO_RECUPERACAO);
        float progresso = t_recup / dur_recup;
        if (progresso > 1.0f)
            progresso = 1.0f;
        float novo_valor = alvo + (MAX_SANIDADE - alvo) * progresso;
        if (novo_valor > san->valor)
            san->valor = novo_valor;
        if (san->valor > MAX_SANIDADE)
            san->valor = MAX_SANIDADE;
        san->regenerando = (progresso < 1.0f) ? 1 : 0;
        if (san->valor >= MAX_SANIDADE)
        {
            san->valor = MAX_SANIDADE;
            san->ciclo_base = san->zumbis_mortos;
            san->regenerando = 0;
        }
    }
    else
        san->regenerando = 0;
}

void desenhar_overlay_sanidade(Sanidade *san)
{
    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS)
        return;
    double agora = al_get_time();
    double sem_matar = agora - san->ultimo_abate;
    float frac_overlay;
    if (sem_matar < TEMPO_INICIO_RECUPERACAO)
        frac_overlay = 1.0f;
    else if (sem_matar < TEMPO_RECUPERACAO_TOTAL)
    {
        float t_fade = (float)(sem_matar - TEMPO_INICIO_RECUPERACAO);
        float dur_fade = (float)(TEMPO_RECUPERACAO_TOTAL - TEMPO_INICIO_RECUPERACAO);
        frac_overlay = 1.0f - (t_fade / dur_fade);
        if (frac_overlay < 0.0f)
            frac_overlay = 0.0f;
    }
    else
        return;

    float pulso = 0.5f + 0.5f * sinf((float)agora * 5.0f);
    unsigned char alpha = (unsigned char)((60.0f + 120.0f * frac_overlay) * pulso * frac_overlay);
    if (alpha == 0)
        return;
    al_draw_filled_rectangle(0, 0, LARGURA, ALTURA, al_map_rgba(200, 0, 0, alpha));
}

void desenhar_aviso_sanidade(Sanidade *san, ALLEGRO_FONT *fonte, double t)
{
    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS || san->game_over)
        return;
    double sem_matar = t - san->ultimo_abate;
    if (sem_matar >= TEMPO_RECUPERACAO_TOTAL)
        return;
    if ((int)(t / 0.6) % 2 == 0)
        return;
    float tx = LARGURA - 616.0f, ty = ALTURA - 105.0f;
    al_draw_text(fonte, al_map_rgba(0, 0, 0, 200), tx + 2, ty + 2, ALLEGRO_ALIGN_CENTER, "INSANIDADE!");
    al_draw_text(fonte, al_map_rgba(0, 0, 0, 200), tx + 2, ty + 38, ALLEGRO_ALIGN_CENTER, "Pare de matar!");
    al_draw_text(fonte, al_map_rgb(255, 60, 60), tx, ty, ALLEGRO_ALIGN_CENTER, "INSANIDADE!");
    al_draw_text(fonte, al_map_rgb(255, 180, 180), tx, ty + 38, ALLEGRO_ALIGN_CENTER, "Pare de matar!");
}

void desenhar_mensagem_central_insanidade(Sanidade *san, ALLEGRO_FONT *fonte,
                                          ALLEGRO_FONT *fonte_hud, double t)
{
    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS || san->game_over)
        return;
    double sem_matar = t - san->ultimo_abate;
    if (sem_matar >= TEMPO_RECUPERACAO_TOTAL)
        return;
    float frac_overlay;
    if (sem_matar < TEMPO_INICIO_RECUPERACAO)
        frac_overlay = 1.0f;
    else
    {
        float t_fade = (float)(sem_matar - TEMPO_INICIO_RECUPERACAO);
        float dur_fade = (float)(TEMPO_RECUPERACAO_TOTAL - TEMPO_INICIO_RECUPERACAO);
        frac_overlay = 1.0f - (t_fade / dur_fade);
        if (frac_overlay < 0.0f)
            frac_overlay = 0.0f;
    }
    if (frac_overlay < 0.05f)
        return;
    float pulso = 0.7f + 0.3f * sinf((float)t * 3.0f);
    unsigned char alpha_txt = (unsigned char)(255.0f * frac_overlay * pulso);
    float cx = LARGURA / 2.0f, cy = ALTURA / 2.0f - 40.0f, box_w = 840.0f, box_h = 110.0f;
    al_draw_filled_rounded_rectangle(cx - box_w / 2.0f, cy - 20.0f, cx + box_w / 2.0f, cy + box_h,
                                     12, 12, al_map_rgba(0, 0, 0, (unsigned char)(160.0f * frac_overlay)));
    al_draw_text(fonte, al_map_rgba(255, 40, 40, alpha_txt), cx, cy, ALLEGRO_ALIGN_CENTER, "VOCE ESTA INSANO!");
    al_draw_text(fonte_hud, al_map_rgba(255, 200, 200, alpha_txt), cx, cy + 52.0f, ALLEGRO_ALIGN_CENTER,
                 "Pare de eliminar inimigos ou perca sua mente!");
}

/* ================================================================== */
/*  HUD – TEMPO / HORDA / ATAQUES / DASH                               */
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
/*  PROJÉTIL DE ÁCIDO                                                   */
/* ================================================================== */
void projetil_acido_spawn(float x, float y, int direcao)
{
    for (int i = 0; i < MAX_PROJETEIS_ACIDO; i++)
    {
        if (!g_projeteis_acido[i].ativo)
        {
            g_projeteis_acido[i].x = x + (direcao == 0 ? 30.0f : -30.0f);
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

void projeteis_acido_atualizar(ALLEGRO_BITMAP *mapa, Jogador *jog, VidaStatus *vidas, Sons *sons)
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

        float vx = (p->direcao == 0) ? ACIDO_PROJETIL_VEL : -ACIDO_PROJETIL_VEL;
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

            float kb_dir = (p->direcao == 0) ? 1.0f : -1.0f;
            jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X * 0.5f;
            jog->direcao = (p->direcao == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

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
/*  EXPLOSÃO DE ÁCIDO                                                   */
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
            return;
        }
    }
}

void explosoes_acidas_atualizar(Jogador *jog, VidaStatus *vidas, Sons *sons)
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

                float kb_dir = (dx >= 0) ? 1.0f : -1.0f;
                jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X;
                jog->mov.vel_y = -6.0f;
                jog->direcao = (kb_dir > 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

                if (jog->estado != SAM_DEAD)
                {
                    jog->estado = SAM_DEAD;
                    jog->frame_dead = 0;
                    jog->morte_animando = 1;
                    jog->som_morrendo_tocado = 0;
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

/* ================================================================== */
/*  HORDA – INIT                                                        */
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

static void spawnar_zumbi(Horda *h, Sons *sons, int s, float sx_, float sy_, int spawn_tipo)
{
    Inimigo *z = &h->pool[s];
    z->x = sx_;
    z->y = sy_;
    z->vel_y = 0.0f;
    z->x_inicial = sx_;
    z->y_inicial = sy_;
    z->patrol_base = sx_;
    z->patrol_dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
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

    int roll = rand() % 100;

    if (roll < 15)
    {
        /* limita a no máximo 2 zumbis ácidos vivos simultaneamente */
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
/*  HORDA – SPAWN                                                       */
/* ================================================================== */
void horda_atualizar_spawn(Horda *h, Sons *sons, float jogador_y)
{
    if (h->fase_concluida || h->total_spawned >= TOTAL_ZUMBIS_FASE)
        return;
    if (jogador_y < SPAWN_MIN_JOGADOR_Y)
        return;

    /* bloqueia novo spawn enquanto houver zumbi ácido vivo na tela */
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
        int para_spawn = (restam < 5) ? restam : 5;
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
/*  HORDA – FÍSICA                                                      */
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
/*  HORDA – MOVIMENTO / IA                                              */
/* ================================================================== */
void horda_atualizar_movimento(Horda *h, Jogador *j, Sons *sons, ALLEGRO_BITMAP *mapa)
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

            int max_f = (z->estado == ZUM_ATTACK) ? FRAMES_ZUMBI_ATTACK1 : FRAMES_ZUMBI_BITE;
            float vel_an = (z->tipo == 1) ? 0.15f : 0.10f;
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
        float jy_ref = (j->no_chao) ? jy : j->y_chao;
        int zumbi_abaixo = (z->y > jy_ref + ZUMBI_ABAIXO_MARGEM);
        int jogador_alto = (jy_ref < NIVEL_ALTO_Y);

        /* patrulha quando jogador está abaixo ou longe no alto */
        if ((!jogador_morto && zumbi_abaixo) ||
            (!jogador_morto && jogador_alto &&
             !(fabsf(jog_cx - zumbi_cx) < 250.0f && fabsf(jog_cy - zumbi_cy) < 250.0f)))
        {
            if (agora - z->tempo_ataque > 1.5)
            {
                z->tempo_ataque = agora;
                int r = rand() % 3;
                z->patrol_dir = (r == 0) ? -1.0f : (r == 1) ? 1.0f
                                                            : 0.0f;
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
                z->direcao = (z->patrol_dir > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
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
                float nx_z = z->x - (dx_signed > 0 ? passo : -passo);
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                z->direcao = (dx_signed > 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                z->estado = ZUM_WALK;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ACIDO_WALK)
                    z->frame = 0.0f;
            }
            else if (dist > ZUMBI_ACIDO_DIST_MAX)
            {
                float passo = z->velocidade * 0.5f;
                float nx_z = z->x + (dx_signed > 0 ? passo : -passo);
                if (!zumbi_colide_horizontal(mapa, nx_z, z->y))
                    z->x = nx_z;
                z->direcao = (dx_signed > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
                z->estado = ZUM_WALK;
                z->frame += 0.04f;
                if (z->frame >= FRAMES_ACIDO_WALK)
                    z->frame = 0.0f;
            }
            else
            {
                z->direcao = (dx_signed > 0) ? 0 : ALLEGRO_FLIP_HORIZONTAL;
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
                z->estado = (z->tipo == 1) ? ZUM_BITE : ZUM_ATTACK;
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
            z->estado = (z->tipo == 1) ? ZUM_RUN : ZUM_WALK;
            float va = (z->tipo == 1) ? 0.14f : 0.07f;
            z->frame += va;
            int mf = (z->tipo == 1) ? FRAMES_ZUMBI_RUN : FRAMES_ZUMBI_WALK;
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
    float atk_x = (jog->direcao == 0) ? jog_cx + offset : jog_cx - atk_w - offset;

    /* destrói projéteis de ácido que estiverem dentro da hitbox da espadada */
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

        if (atk_x < hzx + hzw && atk_x + atk_w > hzx && atk_y < hzy + hzh && atk_y + atk_h > hzy)
        {
            tocar(sons->acerto_zumbi);
            int dano = (jog->tipo_ataque == 3) ? 2 : 1;
            z->vida -= dano;
            if (z->vida < 0)
                z->vida = 0;
            z->atingido_no_ataque = 1;
            z->ultimo_dano_recebido = agora;
            jog->acertos_no_swing++;

            float kb_dir = (jog->direcao == 0) ? 1.0f : -1.0f;
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
/*  HORDA – DANO AO JOGADOR                                             */
/* ================================================================== */
void horda_verificar_dano_jogador(Horda *h, Jogador *jog, VidaStatus *vidas, Sons *sons)
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
            continue; /* dano do ácido vem só do projétil/explosão */
        if (z->estado != ZUM_ATTACK && z->estado != ZUM_BITE)
            continue;
        if (z->dano_aplicado)
            continue;

        int frame_ativo = (int)z->frame;
        int inicio_dano = (z->estado == ZUM_BITE) ? 4 : 2;
        if (frame_ativo < inicio_dano)
            continue;

        float body_x = z->x + ZUMBI_HBX_OFFSET_X;
        float body_y = z->y + ZUMBI_HBX_OFFSET_Y;
        float body_centro = body_x + ZUMBI_HBX_W / 2.0f;
        float jog_centro = jog->mov.x + HITBOX_W / 2.0f;
        float atk_w = alcance_golpe + ZUM_ATK_REACH;
        float atk_x = (body_centro < jog_centro) ? body_x + ZUMBI_HBX_W : body_x - atk_w;
        float atk_y = body_y;

        float jx = jog->mov.x, jy = jog->mov.y, jw = HITBOX_W, jh = HITBOX_H;
        int acertou = (atk_x < jx + jw && atk_x + atk_w > jx && atk_y < jy + jh && atk_y + ZUM_ATK_H > jy);

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

            float kb_dir = (body_centro < jog_centro) ? 1.0f : -1.0f;
            jog->mov.x += kb_dir * KNOCKBACK_SAMURAI_X;
            jog->mov.vel_y = -6.0f;
            jog->direcao = (body_centro < jog_centro) ? ALLEGRO_FLIP_HORIZONTAL : 0;
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
/*  HORDA – DESENHO                                                     */
/* ================================================================== */
void horda_desenhar(Horda *h, ZumbiSprites *spr, ZumbiAcidoSprites *spr_acido)
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
            sheet = (z->tipo == 2) ? spr_acido->walk : spr->walk;

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
            al_draw_tinted_scaled_bitmap(sheet, tint_acido,
                                         fz * ZUMBI_ACIDO_SRC_W, 0,
                                         ZUMBI_ACIDO_SRC_W, ZUMBI_ACIDO_SRC_H,
                                         z->x, z->y - 40,
                                         ZUMBI_ACIDO_DRAW_W, ZUMBI_ACIDO_DRAW_H,
                                         z->direcao);
        }
        else
        {
            ALLEGRO_COLOR tint = (z->tipo == 1) ? al_map_rgb(255, 120, 120)
                                                : al_map_rgb(255, 255, 255);
            al_draw_tinted_scaled_bitmap(sheet, tint,
                                         fz * ZUMBI_SRC_W, 0, ZUMBI_SRC_W, ZUMBI_SRC_H,
                                         z->x, z->y, ZUMBI_DRAW_W, ZUMBI_DRAW_H,
                                         z->direcao);
        }

        if (z->estado != ZUM_DEAD)
        {
            float draw_w = (z->tipo == 2) ? ZUMBI_ACIDO_DRAW_W : ZUMBI_DRAW_W;
            float draw_y = (z->tipo == 2) ? (z->y - 40) : z->y;
            float bw2 = 58.0f;
            float bx2 = z->x + draw_w / 2.0f - bw2 / 2.0f, by2 = draw_y + 4.0f;
            float vida_pct = (float)z->vida / 5.0f;
            al_draw_filled_rectangle(bx2, by2, bx2 + bw2, by2 + 6, al_map_rgb(80, 0, 0));
            al_draw_filled_rectangle(bx2, by2, bx2 + bw2 * vida_pct, by2 + 6, al_map_rgb(255, 0, 0));
        }
    }
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
    if (jog->estado == SAM_DEAD || jog->estado == SAM_HURT || jog->estado == SAM_DASH)
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
            if (j_now && !j_ant && !jogador.atacando && !roda_aberta &&
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
            if (jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD && !jogador.dash_fuga_ativo)
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
            explosoes_acidas_atualizar(&jogador, vetor_vidas, &sons);

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