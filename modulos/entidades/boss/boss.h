#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "../../tipos.h"
#include "../../sistemas/sons/sons.h"

/* ================================================================== */
/*  Dimensões e hitbox do boss (valores escalados  sprite 512x512)    */
/* ================================================================== */
#define BOSS_SPRITE_W        128
#define BOSS_SPRITE_H        128
#define BOSS_ESCALA          4.0f

#define BOSS_HBX_W           180.0f
#define BOSS_HBX_H           290.0f

#define BOSS_HBX_OFFSET_X    ((BOSS_SPRITE_W * BOSS_ESCALA - BOSS_HBX_W) / 2.0f)
#define BOSS_HBX_OFFSET_Y    220.0f

#define BOSS_SPRITE_OLHA_DIREITA 0

#define BOSS_FRAMES_IDLE      6
#define BOSS_FRAMES_WALK     12
#define BOSS_FRAMES_ATTACK   15
#define BOSS_FRAMES_DAMAGE    5
#define BOSS_FRAMES_DEATH    22

#define BOSS_HP_MAX          60 
#define BOSS_VELOCIDADE        1.8f
#define BOSS_DANO_ATAQUE      25
#define BOSS_ALCANCE_ATAQUE  80.0f  
#define BOSS_DIST_MINIMA     40.0f
#define BOSS_DELAY_ATAQUE      2.2
#define BOSS_DELAY_DANO_JOGADOR 1.2

#define BOSS_FASE2_HP_PCT    0.60f
#define BOSS_FASE3_HP_PCT    0.30f
#define BOSS_VEL_FASE2       2.2f
#define BOSS_VEL_FASE3       3.0f
#define BOSS_DANO_FASE2      35
#define BOSS_DANO_FASE3      50

#define BOSS_ANIM_IDLE_VEL   0.10f
#define BOSS_ANIM_WALK_VEL   0.18f
#define BOSS_ANIM_ATAQUE_VEL 0.20f
#define BOSS_ANIM_DANO_VEL   0.22f
#define BOSS_ANIM_DEATH_VEL  0.14f

#define BOSS_SPAWN_VEL_INICIAL  8.0f
#define BOSS_SPAWN_OFFSET_Y   700.0f

/* ================================================================== */
/*  Estados                                                             */
/* ================================================================== */
typedef enum {
    BOSS_SPAWN = 0,
    BOSS_IDLE,
    BOSS_WALK,
    BOSS_ATTACK,
    BOSS_DAMAGE,
    BOSS_DEATH,
    BOSS_MORTO
} BossEstado;

/* ================================================================== */
/*  Sprites                                                             */
/* ================================================================== */
typedef struct {
    ALLEGRO_BITMAP *idle;
    ALLEGRO_BITMAP *walk;
    ALLEGRO_BITMAP *attack;
    ALLEGRO_BITMAP *damage;
    ALLEGRO_BITMAP *death;
} BossSprites;

/* ================================================================== */
/*  Entidade                                                            */
/* ================================================================== */
typedef struct {
    float x, y;
    float vel_y;
    int   direcao;
    int   no_chao;

    BossEstado estado;
    int   fase;

    float frame;
    int   hp;
    int   hp_max;

    int   ativo;
    int   dano_aplicado;

    double ultimo_ataque;
    double hurt_inicio;

    int   morte_animando;
    int   som_morte_tocado;
    int   som_spawn_tocado;
    int   som_fase2_tocado;
    int   som_fase3_tocado;

    double ultimo_dano_recebido;

    float spawn_y_alvo;
    float spawn_vel;
} Boss;

/* ================================================================== */
/*  API                                                                 */
/* ================================================================== */
void boss_init             (Boss *b, float x, float y);
int  boss_sprites_carregar (BossSprites *spr);
void boss_sprites_destruir (BossSprites *spr);

void boss_atualizar        (Boss *b, Jogador *jogador, Sons *sons,
                            ALLEGRO_BITMAP *mapa_colisao, VidaStatus *vidas);

int  boss_receber_dano     (Boss *b, int dano, Sons *sons);

void boss_verificar_dano_jogador(Boss *b, Jogador *jogador,
                                 VidaStatus *vidas, Sons *sons);

void boss_desenhar         (const Boss *b, const BossSprites *spr);
void boss_desenhar_hud     (const Boss *b, ALLEGRO_FONT *fonte_hud);
int  boss_derrotado        (const Boss *b);

#endif /* BOSS_H */