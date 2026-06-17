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
#define SPRITE_SRC_W    96
#define SPRITE_SRC_H    60

#define SPRITE_SCALE    1.95f
#define DRAW_W          (SPRITE_SRC_W * SPRITE_SCALE)
#define DRAW_H          (SPRITE_SRC_H * SPRITE_SCALE)

#define HITBOX_W        50
#define HITBOX_H        80
#define HITBOX_OFFSET_X 70
#define HITBOX_OFFSET_Y 40

#define ATTACK_DRAW_W   DRAW_W
#define ATTACK_DRAW_H   DRAW_H

/* ================================================================== */
/*  FÍSICA DO SAMURAI                                                   */
/* ================================================================== */
#define VELOCIDADE   4.0f
#define GRAVIDADE    0.7f
#define FORCA_PULO  -13.05f
#define MAX_QUEDA    18.0f

/* ================================================================== */
/*  SPRITE DO ZUMBI                                                     */
/* ================================================================== */
#define ZUMBI_SRC_W  96
#define ZUMBI_SRC_H  96
#define ZUMBI_DRAW_W 120
#define ZUMBI_DRAW_H 120

#define ZUMBI_HBX_OFFSET_X 38
#define ZUMBI_HBX_OFFSET_Y 36
#define ZUMBI_HBX_W        44
#define ZUMBI_HBX_H        90

#define ZUMBI_OFFSET_X 0
#define ZUMBI_OFFSET_Y 0

/* ================================================================== */
/*  FÍSICA DO ZUMBI                                                     */
/* ================================================================== */
#define GRAVIDADE_ZUMBI  0.55f
#define MAX_QUEDA_ZUMBI  14.0f

/* ================================================================== */
/*  FRAMES DE ANIMAÇÃO — SAMURAI                                        */
/* ================================================================== */
#define FRAMES_IDLE  6
#define FRAMES_RUN   8
#define FRAMES_JUMP  12
#define FRAMES_HURT  2
#define FRAMES_DEAD  3

/* ================================================================== */
/*  FRAMES DE ANIMAÇÃO — ZUMBI                                          */
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
/*  VIDA E ESTAMINA                                                     */
/* ================================================================== */
#define MAX_VIDAS     5
#define MAX_ESTAMINA  10.0f
#define CUSTO_PULO    1.0f
#define CUSTO_ATK1    1.0f
#define CUSTO_ATK2    2.0f
#define CUSTO_ATK3    5.0f
#define RECARGA_ESTAMINA 0.02f

/* ================================================================== */
/*  SANIDADE                                                            */
/* ================================================================== */
#define MAX_SANIDADE             100.0f
#define TOTAL_ZUMBIS_GAME_OVER   15
#define AVISO_ZUMBIS             13
#define QUEDA_POR_ZUMBI          (MAX_SANIDADE / (float)TOTAL_ZUMBIS_GAME_OVER)
#define SANIDADE_OVERLAY_LIM     25.0f
#define TEMPO_INICIO_RECUPERACAO 10.0
#define TEMPO_RECUPERACAO_TOTAL  20.0

/* ================================================================== */
/*  HORDA                                                               */
/* ================================================================== */
#define MAX_ZUMBIS_TELA    20
#define TOTAL_ZUMBIS_FASE  50
#define ZUMBIS_POR_ONDA    5
#define INTERVALO_ONDA     230

#define SPAWN_DIR_X_MIN  1700
#define SPAWN_DIR_X_MAX  1800
#define SPAWN_DIR_Y      540

#define SPAWN_TOP_LEFT_X 60
#define SPAWN_TOP_LEFT_Y 90

#define SPAWN_MIN_JOGADOR_Y  300
#define SPAWN_TOP_Y_THRESHOLD 300

/* ================================================================== */
/*  PATRULHA E PERCEPÇÃO                                                */
/* ================================================================== */
#define PATROL_AREA_X_MIN  100
#define PATROL_AREA_X_MAX  1800
#define NIVEL_ALTO_Y       380
#define PATRULHA_VEL       1.5f
#define PATRULHA_DIST      80.0f
#define ZUMBI_ABAIXO_MARGEM 55

/* ================================================================== */
/*  COMBATE — TEMPOS E KNOCKBACK                                        */
/* ================================================================== */
#define TEMPO_INVUL            1.0
#define KNOCKBACK_ZUMBI_DURACAO  0.7
#define KNOCKBACK_SAMURAI_DURACAO 0.5
#define KNOCKBACK_ZUMBI_X      12.0f
#define KNOCKBACK_SAMURAI_X    28.0f
#define DELAY_ATAQUE_12        0.35
#define DELAY_ATAQUE_3         0.45

/* ================================================================== */
/*  HITBOX DE ATAQUE — SAMURAI                                          */
/* ================================================================== */
#define SAM_ATK_FRAME_INICIO 2
#define SAM_ATK_FRAME_FIM    4
#define SAM_ATK_W            45
#define SAM_ATK_H            45
#define SAM_ATK_REACH        15

/* ================================================================== */
/*  HITBOX DE ATAQUE — ZUMBI                                            */
/* ================================================================== */
#define ZUM_ATK_W     60
#define ZUM_ATK_H     45
#define ZUM_ATK_REACH 10

/* ================================================================== */
/*  DASH DO ATAQUE 3                                                    */
/* ================================================================== */
#define DASH_ATK3_DIST 84.0f

/* ================================================================== */
/*  DASH DE FUGA (tecla E)                                              */
/* ================================================================== */
#define DASH_FUGA_DIST      200.0f
#define DASH_FUGA_CUSTO     5.0f
#define DASH_FUGA_DELAY     0.6
#define DASH_FUGA_FRAMES_RUN 8

/* ================================================================== */
/*  POÇÃO DE VIDA                                                       */
/* ================================================================== */
#define POCAO_SPAWN_X_MIN   435
#define POCAO_SPAWN_X_MAX   1870
#define POCAO_SPAWN_Y       100
#define POCAO_LARGURA       48
#define POCAO_ALTURA        48
#define POCAO_GRAVIDADE     0.45f
#define POCAO_MAX_QUEDA     14.0f
#define POCAO_RECUPERA_VIDAS 4
#define POCAO_HITBOX_W      40
#define POCAO_HITBOX_H      40

/* ================================================================== */
/*  SONS — INTERVALO MÍNIMO ENTRE REPRODUÇÕES                          */
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

/* ---- Vida ---- */
typedef struct
{
    int  ativa;
    char status[20];
} VidaStatus;

/* ---- Física base ---- */
typedef struct
{
    float x, y, vel_y;
} Movimento;

/* ---- Jogador (Samurai) ---- */
typedef struct
{
    Movimento mov;

    float frame;            /* frame atual da animação geral */
    float frame_ataque;     /* frame atual da animação de ataque */
    float frame_hurt;       /* frame atual da animação de dano */
    float frame_dead;       /* frame atual da animação de morte */

    int no_chao;            /* 1 se está tocando o chão */
    int direcao;            /* 0 = direita, ALLEGRO_FLIP_HORIZONTAL = esquerda */
    int movendo;            /* 1 se está se movendo horizontalmente */
    int atacando;           /* 1 durante o swing */
    int tipo_ataque;        /* 1, 2 ou 3 */
    int morto;              /* flag permanente de morte */
    int morte_animando;     /* 1 enquanto a animação de morte roda */
    int acertos_no_swing;   /* quantos zumbis foram atingidos no swing atual */

    EstadoSamurai estado;

    double ultimo_dano;     /* timestamp do último dano sofrido (invulnerabilidade) */
    double ultimo_ataque;   /* timestamp do último ataque (delay entre ataques) */
    double hurt_inicio;     /* timestamp em que o estado HURT começou */

    float estamina;

    float y_chao;           /* último Y registrado enquanto estava no chão */

    /* ---- Dash do ataque 3 ---- */
    int   dash_ativo;
    float dash_dist;        /* distância restante do dash */

    /* ---- Dash de fuga (tecla E) ---- */
    int    dash_fuga_ativo;
    float  dash_fuga_dist;
    double dash_fuga_ultimo; /* timestamp do último dash de fuga */
    float  dash_fuga_frame;  /* frame da animação de corrida durante o dash */

    /* ---- Controle de sons ---- */
    int    som_ataque_tocado;
    int    dash_som_tocado;
    int    som_morrendo_tocado;
    double ultimo_passo_sam; /* timestamp do último som de passos */
} Jogador;

/* ---- Inimigo (Zumbi) ---- */
typedef struct
{
    float x, y;
    float x_inicial, y_inicial;
    float vel_y;
    float velocidade;
    float frame;

    int direcao;   /* 0 = direita, ALLEGRO_FLIP_HORIZONTAL = esquerda */
    int vivo;
    int vida;
    int tipo;      /* 0 = normal, 1 = velocista */

    EstadoZumbi estado;

    /* ---- Patrulha ---- */
    float patrol_dir;   /* -1, 0 ou +1 */
    float patrol_base;  /* X de referência da patrulha */

    /* ---- Ataque ao jogador ---- */
    int    atacando_jogador;
    double tempo_ataque;    /* timestamp do início do ataque atual */
    int    dano_aplicado;   /* 1 após o dano ser aplicado no frame ativo */
    int    atingido_no_ataque; /* 1 se já foi atingido neste swing do samurai */

    /* ---- Dano recebido ---- */
    double ultimo_dano_recebido; /* para invulnerabilidade do zumbi */
    double tempo_hurt;           /* timestamp do início do estado HURT */

    /* ---- Stun (dash de fuga) ---- */
    int    stunado;
    double tempo_stun;
    int    ja_stunado_no_dash; /* evita aplicar stun duas vezes no mesmo dash */

    /* ---- Spawn ---- */
    int spawn_tipo;

    /* ---- Controle de sons ---- */
    int ataque_som_tocado;
    int ataque_resultado;   /* 0 = pendente, 1 = acertou, 2 = errou */
} Inimigo;

/* ---- Horda ---- */
typedef struct
{
    Inimigo pool[MAX_ZUMBIS_TELA];

    int total_spawned;   /* quantos já foram spawnados na fase */
    int total_mortos;    /* quantos foram mortos */
    int timer_onda;      /* contador regressivo até o próximo spawn */
    int fase_concluida;  /* 1 quando total_mortos >= TOTAL_ZUMBIS_FASE */

    /* controle de balanceamento de spawn por lado */
    int top_spawned;
    int top_ativo;
    int spawns_esq;
    int spawns_dir;
} Horda;

/* ---- Sanidade ---- */
typedef struct
{
    float  valor;           /* 0..MAX_SANIDADE */
    int    zumbis_mortos;   /* total acumulado desde o início */
    int    ciclo_base;      /* base do ciclo atual (reseta ao recuperar 100%) */
    int    game_over;       /* 1 quando a sanidade chega a zero */
    double ultimo_abate;    /* timestamp do último zumbi morto */
    int    regenerando;     /* 1 durante a recuperação gradual */
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
    double inicio;          /* al_get_time() no início da fase */
    double atual;           /* tempo corrido (atualizado a cada frame) */
    double fim;             /* tempo final ao concluir a fase */
    int    ativo;           /* 0 após a fase terminar */
    float  ranking[10];     /* top 10 tempos (ordenados crescente) */
    int    quantidade_scores;
} Temporizador;

/* ---- Partícula decorativa ---- */
typedef struct
{
    float x, y;       /* posição atual em pixels */
    float vy;         /* velocidade vertical (pixels/frame) */
    float fase;       /* fase inicial do pulso (0..2*PI) */
    float escala;     /* tamanho do ponto (1..3 px) */
    float y_origem;   /* y de reset quando sai da tela */
} Particula;

/* ---- Sprite sheets do Samurai ---- */
typedef struct
{
    ALLEGRO_BITMAP *idle;
    ALLEGRO_BITMAP *run;
    ALLEGRO_BITMAP *jump;
    ALLEGRO_BITMAP *attack1;
    ALLEGRO_BITMAP *attack2;
    ALLEGRO_BITMAP *attack3;
    ALLEGRO_BITMAP *hurt;
    ALLEGRO_BITMAP *dead;
} SamuraiSprites;

/* ---- Sprite sheets do Zumbi ---- */
typedef struct
{
    ALLEGRO_BITMAP *walk;
    ALLEGRO_BITMAP *run;
    ALLEGRO_BITMAP *attack1;
    ALLEGRO_BITMAP *attack2;
    ALLEGRO_BITMAP *attack3;
    ALLEGRO_BITMAP *bite;
    ALLEGRO_BITMAP *hurt;
    ALLEGRO_BITMAP *dead;
    ALLEGRO_BITMAP *idle;
} ZumbiSprites;

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

    /* Instância da música de fundo */
    ALLEGRO_SAMPLE_ID padrao_id;
    int padrao_tocando;
} Sons;

#endif /* TIPOS_H */
