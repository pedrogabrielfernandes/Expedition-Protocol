#ifndef ZUMBI_H
#define ZUMBI_H

#include <allegro5/allegro.h>
#include "../../tipos.h"

extern ProjetilAcido g_projeteis_acido[MAX_PROJETEIS_ACIDO];
extern ExplosaoAcida g_explosoes_acidas[MAX_EXPLOSOES_ACIDAS];
extern unsigned char g_explosao_acida_hit[MAX_EXPLOSOES_ACIDAS][MAX_ZUMBIS_TELA];

/* ================================================================== */
/*  HORDA                                                              */
/* ================================================================== */
void horda_init                  (Horda *h);
void horda_atualizar_spawn       (Horda *h, Sons *sons, float jogador_y);
void horda_atualizar_fisica      (Horda *h, ALLEGRO_BITMAP *mapa);
void horda_atualizar_movimento   (Horda *h, Jogador *j, Sons *sons, ALLEGRO_BITMAP *mapa);
void horda_verificar_ataque(Horda *h, Jogador *jog, Sanidade *san, Sons *sons, ALLEGRO_BITMAP *mapa);
void horda_verificar_dano_jogador(Horda *h, Jogador *jog, VidaStatus *vidas, Sons *sons);
void horda_desenhar              (Horda *h, ZumbiSprites *spr, ZumbiAcidoSprites *spr_acido);

/* ================================================================== */
/*  PROJÉTIL DE ÁCIDO                                                  */
/* ================================================================== */
void projetil_acido_spawn       (float x, float y, int direcao);
void projeteis_acido_atualizar  (ALLEGRO_BITMAP *mapa, Jogador *jog, VidaStatus *vidas, Sons *sons);
void projeteis_acido_desenhar   (ALLEGRO_BITMAP *spr);

/* ================================================================== */
/*  EXPLOSÃO DE ÁCIDO                                                  */
/* ================================================================== */
void explosao_acida_spawn       (float x, float y);
void explosoes_acidas_atualizar (Horda *h, Jogador *jog, VidaStatus *vidas, Sanidade *san, Sons *sons);
void explosoes_acidas_desenhar  (ALLEGRO_BITMAP *frames[]);

#endif /* ZUMBI_H */