#ifndef TIPOS_H
#define TIPOS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

/* ================================================================== */
/*  TELA E TEMPO                                                        */
/* ================================================================== */
#define LARGURA 1920
#define ALTURA  1080
#define FPS     60

/* ================================================================== */
/*  SPRITE DO SAMURAI                                                   */
/* ================================================================== */
#define SPRITE_SRC_W   96
#define SPRITE_SRC_H   60
#define SPRITE_SCALE   1.95f
#define DRAW_W         (SPRITE_SRC_W * SPRITE_SCALE)
#define DRAW_H         (SPRITE_SRC_H * SPRITE_SCALE)

#define HITBOX_W        50
#define HITBOX_H        80
#define HITBOX_OFFSET_X 70
#define HITBOX_OFFSET_Y 40

#define ATTACK_DRAW_W  DRAW_W
#define ATTACK_DRAW_H  DRAW_H

/* ================================================================== */
/*  FÍSICA DO SAMURAI                                                   */
/* ================================================================== */
#define VELOCIDADE   4.0f
#define GRAVIDADE    0.7f
#define FORCA_PULO  -13.05f
#define MAX_QUEDA    18.0f

/* ================================================================== */
/*  SPRITE DO ZUMBI NORMAL                                              */
/* ================================================================== */
#define ZUMBI_SRC_W        96
#define ZUMBI_SRC_H        96
#define ZUMBI_DRAW_W      120
#define ZUMBI_DRAW_H      120

/* Hitbox do zumbi normal */
#define ZUMBI_HBX_OFFSET_X  38
#define ZUMBI_HBX_OFFSET_Y  42
#define ZUMBI_HBX_W         44
#define ZUMBI_HBX_H         80

#define ZUMBI_OFFSET_X  0
#define ZUMBI_OFFSET_Y  0

/* ================================================================== */
/*  FÍSICA DO ZUMBI                                                     */
/* ================================================================== */
#define GRAVIDADE_ZUMBI   0.55f
#define MAX_QUEDA_ZUMBI   14.0f

/* ================================================================== */
/*  FRAMES DE ANIMAÇÃO ? SAMURAI                                        */
/* ================================================================== */
#define FRAMES_IDLE   6
#define FRAMES_RUN    8
#define FRAMES_JUMP   12
#define FRAMES_HURT   2
#define FRAMES_DEAD   3

/* ================================================================== */
/*  FRAMES DE ANIMAÇÃO ? ZUMBI NORMAL                                   */
/* ================================================================== */
#define FRAMES_ZUMBI_WALK    8
#define FRAMES_ZUMBI_RUN     7
#define FRAMES_ZUMBI_ATTACK1 5
#define FRAMES_ZUMBI_ATTACK2 4
#define FRAMES_ZUMBI_ATTACK3 5
#define FRAMES_ZUMBI_BITE    11
#define FRAMES_ZUMBI_HURT    3
#define FRAMES_ZUMBI_DEAD    5
#define FRAMES_ZUMBI_IDLE    8

/* ================================================================== */
/*  ZUMBI ÁCIDO                                                         */
/* ================================================================== */
#define ACIDO_HBX_OFFSET_X   60
#define ACIDO_HBX_OFFSET_Y   48
#define ACIDO_HBX_W          60
#define ACIDO_HBX_H          90

#define FRAMES_ACIDO_ATTACK   4
#define FRAMES_ACIDO_DEAD     5
#define FRAMES_ACIDO_HURT     4
#define FRAMES_ACIDO_IDLE     6
#define FRAMES_ACIDO_WALK    10

#define ZUMBI_ACIDO_SRC_W   128
#define ZUMBI_ACIDO_SRC_H   128
#define ZUMBI_ACIDO_DRAW_W  160
#define ZUMBI_ACIDO_DRAW_H  160

#define ZUMBI_ACIDO_DIST_MIN          180.0f
#define ZUMBI_ACIDO_DIST_MAX          380.0f
#define ZUMBI_ACIDO_COOLDOWN_ATAQUE   2.5
#define ZUMBI_ACIDO_FRAME_DISPARO     3

/* ================================================================== */
/*  PROJÉTIL ÁCIDO                                                      */
/* ================================================================== */
#define MAX_PROJETEIS_ACIDO      12
#define FRAMES_ACIDO_PROJETIL     9
#define ACIDO_PROJETIL_SRC_W    128
#define ACIDO_PROJETIL_SRC_H    128
#define ACIDO_PROJETIL_DRAW_W    52
#define ACIDO_PROJETIL_DRAW_H    52
#define ACIDO_PROJETIL_VEL        6.0f
#define ACIDO_PROJETIL_DIST_MAX  500.0f
#define ACIDO_PROJETIL_HBX_W     24
#define ACIDO_PROJETIL_HBX_H     24

/* ================================================================== */
/*  EXPLOSÃO ÁCIDA                                                      */
/* ================================================================== */
#define MAX_EXPLOSOES_ACIDAS    8
#define FRAMES_EXPLOSAO_ACIDA   10
#define EXPLOSAO_ACIDA_RAIO     150.0f
#define EXPLOSAO_ACIDA_DRAW_W   300
#define EXPLOSAO_ACIDA_DRAW_H   300

/* ================================================================== */
/*  VIDA E ESTAMINA                                                     */
/* ================================================================== */
#define MAX_VIDAS       5
#define MAX_ESTAMINA    10.0f
#define CUSTO_PULO      1.0f
#define CUSTO_ATK1      1.0f
#define CUSTO_ATK2      2.0f
#define CUSTO_ATK3      5.0f
#define RECARGA_ESTAMINA 0.02f

/* ================================================================== */
/*  SANIDADE                                                            */
/* ================================================================== */
#define MAX_SANIDADE              100.0f
#define TOTAL_ZUMBIS_GAME_OVER    15
#define AVISO_ZUMBIS              13
#define QUEDA_POR_ZUMBI           (MAX_SANIDADE / (float)TOTAL_ZUMBIS_GAME_OVER)
#define SANIDADE_OVERLAY_LIM      25.0f
#define TEMPO_INICIO_RECUPERACAO  10.0
#define TEMPO_RECUPERACAO_TOTAL   20.0

/* ================================================================== */
/*  HORDA                                                               */
/* ================================================================== */
#define MAX_ZUMBIS_TELA      20
#define TOTAL_ZUMBIS_FASE    50
#define ZUMBIS_POR_ONDA       5
#define INTERVALO_ONDA      230

#define SPAWN_DIR_X_MIN   1700
#define SPAWN_DIR_X_MAX   1800
#define SPAWN_DIR_Y        540

#define SPAWN_TOP_LEFT_X    60
#define SPAWN_TOP_LEFT_Y    90

#define SPAWN_MIN_JOGADOR_Y  300
#define SPAWN_TOP_Y_THRESHOLD 300

/* ================================================================== */
/*  PATRULHA E PERCEPÇÃO                                                */
/* ================================================================== */
#define PATROL_AREA_X_MIN   100
#define PATROL_AREA_X_MAX  1800
#define NIVEL_ALTO_Y        380
#define PATRULHA_VEL        1.5f
#define PATRULHA_DIST       80.0f
#define ZUMBI_ABAIXO_MARGEM 55

/* ================================================================== */
/*  COMBATE ? TEMPOS E KNOCKBACK                                        */
/* ================================================================== */
#define TEMPO_INVUL               1.0
#define KNOCKBACK_ZUMBI_DURACAO   0.7
#define KNOCKBACK_SAMURAI_DURACAO 0.5
#define KNOCKBACK_ZUMBI_X         12.0f
#define KNOCKBACK_SAMURAI_X       28.0f
#define DELAY_ATAQUE_12           0.35
#define DELAY_ATAQUE_3            0.45

/* ================================================================== */
/*  HITBOX DE ATAQUE ? SAMURAI                                          */
/* ================================================================== */
#define SAM_ATK_FRAME_INICIO  2
#define SAM_ATK_FRAME_FIM     4
#define SAM_ATK_W             45
#define SAM_ATK_H             45
#define SAM_ATK_REACH         15

/* ================================================================== */
/*  HITBOX DE ATAQUE ? ZUMBI                                            */
/* ================================================================== */
#define ZUM_ATK_W     60
#define ZUM_ATK_H     45
#define ZUM_ATK_REACH 10

/* ================================================================== */
/*  DASH DO ATAQUE 3                                                    */
/* ================================================================== */
#define DASH_ATK3_DIST  84.0f

/* ================================================================== */
/*  DASH DE FUGA (tecla E)                                              */
/* ================================================================== */
#define DASH_FUGA_DIST      200.0f
#define DASH_FUGA_CUSTO       5.0f
#define DASH_FUGA_DELAY       0.6
#define DASH_FUGA_FRAMES_RUN  8

/* ================================================================== */
/*  POÇÃO DE VIDA                                                       */
/* ================================================================== */
#define POCAO_SPAWN_X_MIN    435
#define POCAO_SPAWN_X_MAX   1870
#define POCAO_SPAWN_Y        100
#define POCAO_LARGURA         48
#define POCAO_ALTURA          48
#define POCAO_GRAVIDADE       0.45f
#define POCAO_MAX_QUEDA      14.0f
#define POCAO_RECUPERA_VIDAS  4
#define POCAO_HITBOX_W        40
#define POCAO_HITBOX_H        40

/* ================================================================== */
/*  SONS ? INTERVALO MÍNIMO ENTRE REPRODUÇÕES                          */
/* ================================================================== */
#define SOM_WALK_SAM_INTERVALO 0.30f

/* ================================================================== */
/*  ENUMS                                                               */
/* ================================================================== */
typedef enum
{
    SAM_IDLE = 0,
    SAM_RUN,
    SAM_JUMP,
    SAM_ATTACK,
    SAM_HURT,
    SAM_DEAD,
    SAM_DASH
} EstadoSamurai;

typedef enum
{
    ZUM_WALK = 0,
    ZUM_RUN,
    ZUM_ATTACK,
    ZUM_BITE,
    ZUM_HURT,
    ZUM_DEAD,
    ZUM_IDLE
} EstadoZumbi;

typedef enum
{
    MENU_JOGAR = 0,
    MENU_SAIR  = 1
} OpcaoMenu;

/* ================================================================== */
/*  STRUCTS                                                             */
/* ================================================================== */

typedef struct { int ativa; char status[20]; } VidaStatus;

typedef struct { float x, y, vel_y; } Movimento;

typedef struct
{
    Movimento mov;

    float frame;
    float frame_ataque;
    float frame_hurt;
    float frame_dead;

    int no_chao;
    int direcao;
    int movendo;
    int atacando;
    int tipo_ataque;
    int morto;
    int morte_animando;
    int acertos_no_swing;

    EstadoSamurai estado;

    double ultimo_dano;
    double ultimo_ataque;
    double hurt_inicio;

    float estamina;
    float y_chao;

    /* Dash ataque 3 */
    int   dash_ativo;
    float dash_dist;

    /* Dash de fuga */
    int    dash_fuga_ativo;
    float  dash_fuga_dist;
    double dash_fuga_ultimo;
    float  dash_fuga_frame;

    /* Sons */
    int    som_ataque_tocado;
    int    dash_som_tocado;
    int    som_morrendo_tocado;
    double ultimo_passo_sam;
} Jogador;

typedef struct
{
    float x, y;
    float x_inicial, y_inicial;
    float vel_y;
    float velocidade;
    float frame;

    int direcao;
    int vivo;
    int vida;
    int tipo;   /* 0=normal, 1=velocista, 2=ácido */

    EstadoZumbi estado;

    float  patrol_dir;
    float  patrol_base;

    int    atacando_jogador;
    double tempo_ataque;
    int    dano_aplicado;
    int    atingido_no_ataque;

    double ultimo_dano_recebido;
    double tempo_hurt;

    int    stunado;
    double tempo_stun;
    int    ja_stunado_no_dash;

    int    spawn_tipo;

    /* Ácido */
    double tempo_morte;
    int    explodiu;

    /* Sons */
    int ataque_som_tocado;
    int ataque_resultado;
} Inimigo;

/* ---- Projétil ácido ---- */
typedef struct
{
    float x, y;
    int   direcao;
    float frame;
    int   ativo;
    float dist_percorrida;
    int   impactou;
} ProjetilAcido;

/* ---- Explosão ácida ---- */
typedef struct
{
    float x, y;
    float frame;
    int   ativo;
} ExplosaoAcida;

/* ---- Horda ---- */
typedef struct
{
    Inimigo pool[MAX_ZUMBIS_TELA];
    int total_spawned;
    int total_mortos;
    int timer_onda;
    int fase_concluida;
    int top_spawned;
    int top_ativo;
    int spawns_esq;
    int spawns_dir;
} Horda;

/* ---- Sanidade ---- */
typedef struct
{
    float  valor;
    int    zumbis_mortos;
    int    ciclo_base;
    int    game_over;
    double ultimo_abate;
    int    regenerando;
} Sanidade;

/* ---- Poção ---- */
typedef struct
{
    float x, y;
    float vel_y;
    int   no_chao;
    int   ativa;
    int   coletada;
} Pocao;

/* ---- Temporizador / Ranking ---- */
typedef struct
{
    double inicio;
    double atual;
    double fim;
    int    ativo;
    float  ranking[10];
    int    quantidade_scores;
} Temporizador;

/* ---- Partícula decorativa ---- */
typedef struct
{
    float x, y;
    float vy;
    float fase;
    float escala;
    float y_origem;
} Particula;

/* ---- Sprite sheets ---- */
typedef struct
{
    ALLEGRO_BITMAP *idle, *run, *jump;
    ALLEGRO_BITMAP *attack1, *attack2, *attack3;
    ALLEGRO_BITMAP *hurt, *dead;
} SamuraiSprites;

typedef struct
{
    ALLEGRO_BITMAP *walk, *run;
    ALLEGRO_BITMAP *attack1, *attack2, *attack3;
    ALLEGRO_BITMAP *bite, *hurt, *dead, *idle;
} ZumbiSprites;

typedef struct
{
    ALLEGRO_BITMAP *walk, *attack, *hurt, *dead, *idle;
} ZumbiAcidoSprites;

/* ---- Sons ---- */
typedef struct
{
    /* Samurai */
    ALLEGRO_SAMPLE *katana12;
    ALLEGRO_SAMPLE *katana_attack3;
    ALLEGRO_SAMPLE *dash;
    ALLEGRO_SAMPLE *acerto_zumbi;
    ALLEGRO_SAMPLE *dano_sofrido;
    ALLEGRO_SAMPLE *pulo;
    ALLEGRO_SAMPLE *walk_sam;
    ALLEGRO_SAMPLE *morrendo;
    ALLEGRO_SAMPLE *caindo;
    ALLEGRO_SAMPLE *dash_fuga;

    /* Zumbi normal */
    ALLEGRO_SAMPLE *dano;
    ALLEGRO_SAMPLE *dano_miss;
    ALLEGRO_SAMPLE *morte_zumbi;
    ALLEGRO_SAMPLE *dano_zumbi;
    ALLEGRO_SAMPLE *zumbi_knockback;

    /* Zumbi velocista */
    ALLEGRO_SAMPLE *som_velocista;
    ALLEGRO_SAMPLE *ataque_velocista;
    ALLEGRO_SAMPLE *dano_velocista;
    ALLEGRO_SAMPLE *morte_velocista;
    ALLEGRO_SAMPLE *dano_sofrido_velocista;

    /* Zumbi ácido */
    ALLEGRO_SAMPLE *acido_impacto;
    ALLEGRO_SAMPLE *cuspindo_acido;
    ALLEGRO_SAMPLE *dano_zumbi_acido;
    ALLEGRO_SAMPLE *explosao_acida;
    ALLEGRO_SAMPLE *explosao_acida2;
    ALLEGRO_SAMPLE *morte_acido;
    ALLEGRO_SAMPLE *impacto_espada;

    /* Jogo / UI */
    ALLEGRO_SAMPLE *padrao;
    ALLEGRO_SAMPLE *jogar;
    ALLEGRO_SAMPLE *esc_som;
    ALLEGRO_SAMPLE *game_over_som;
    ALLEGRO_SAMPLE *pocao_som;
    ALLEGRO_SAMPLE *sair_som;
    ALLEGRO_SAMPLE *finish_som;
    ALLEGRO_SAMPLE *selecao_som;
    ALLEGRO_SAMPLE *horda_som;

    ALLEGRO_SAMPLE_ID padrao_id;
    int padrao_tocando;
} Sons;

#endif /* TIPOS_H */