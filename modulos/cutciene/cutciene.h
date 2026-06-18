#ifndef CUTCIENE_H
#define CUTCIENE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Constantes                                                          */
/* ------------------------------------------------------------------ */
#define CUTSCENE_MAX_DIALOGOS      20
#define CUTSCENE_MAX_CENAS          8

#define FADE_DURACAO               1.2f
#define FUNDO_EXIBE_DURACAO        1.5f
#define CHARS_POR_SEGUNDO          30.0f

#define CAIXA_Y_FUNDO              0.70f
#define CAIXA_ALTURA               160
#define SPRITE_LARGURA             220
#define SPRITE_ALTURA              200
#define CAIXA_MARGEM               20

/* Contagem: intervalo maior para sincronizar melhor com o audio */
#define CONTAGEM_INTERVALO         1.15f

/* Duracao da tela preta entre cena 5 e cena 6 */
#define TELA_PRETA_DURACAO         5.0f

/* Botao pular */
#define BTN_PULAR_MARGEM           18
#define BTN_PULAR_W                140
#define BTN_PULAR_H                36

/* ------------------------------------------------------------------ */
/*  Tipos                                                               */
/* ------------------------------------------------------------------ */
typedef enum {
    FASE_FADE_IN = 0,
    FASE_FUNDO,
    FASE_DIALOGO,
    FASE_CONTAGEM,
    FASE_TELA_PRETA,   /* transicao especial entre cena5 e cena6 */
    FASE_FADE_OUT,
    FASE_FIM
} FaseCena;

typedef struct {
    const char *personagem;
    const char *texto;
    const char *sprite_path;
} Dialogo;

typedef struct {
    const char  *bg_path;
    Dialogo      dialogos[CUTSCENE_MAX_DIALOGOS];
    int          num_dialogos;
    bool         tem_contagem;
    bool         tem_tela_preta_depois; /* true na cena 5 */
    bool         usar_som_alerta;       /* true na cena 3 */
} CenaDef;

typedef struct {
    CenaDef      cenas[CUTSCENE_MAX_CENAS];
    int          num_cenas;

    int          cena_atual;
    int          dialogo_atual;
    FaseCena     fase;
    double       fase_inicio;
    float        alpha;

    float        chars_mostrados;
    bool         texto_completo;

    int          contagem_num;
    double       ultimo_num_tocado;

    ALLEGRO_BITMAP *bg_atual;
    ALLEGRO_BITMAP *sprite_atual;

    ALLEGRO_SAMPLE          *som_dialogo;
    ALLEGRO_SAMPLE          *som_maquina;
    ALLEGRO_SAMPLE          *som_final;
    ALLEGRO_SAMPLE          *som_fim_mundo;
    ALLEGRO_SAMPLE          *som_alerta;
    ALLEGRO_SAMPLE          *som_portal;
    ALLEGRO_SAMPLE          *sons_num[10];
    ALLEGRO_SAMPLE_INSTANCE *inst_maquina;
    ALLEGRO_SAMPLE_INSTANCE *inst_fim_mundo;
    ALLEGRO_SAMPLE_INSTANCE *inst_alerta;

    ALLEGRO_FONT   *fonte_texto;
    ALLEGRO_FONT   *fonte_nome;
    ALLEGRO_FONT   *fonte_contagem;

    bool         finalizada;
    bool         clique_pendente;
    double       ultimo_som_char;
} Cutscene;

/* ------------------------------------------------------------------ */
/*  API publica                                                         */
/* ------------------------------------------------------------------ */
bool cutscene_criar(Cutscene *cs);
void cutscene_update(Cutscene *cs, float dt);
void cutscene_draw(Cutscene *cs);
void cutscene_mouse_click(Cutscene *cs);
void cutscene_mouse_click_pos(Cutscene *cs, int mx, int my); /* usa posicao para detectar botao Pular */
void cutscene_skip(Cutscene *cs);
void cutscene_destruir(Cutscene *cs);
bool cutscene_finalizada(const Cutscene *cs);

#endif /* CUTCIENE_H */