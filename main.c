/* ================================================================== */
/*  main.c    Samurai vs Zumbis                                       */
/* ================================================================== */
#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
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

#include "modulos/entidades/boss/boss.h" /* [BOSS] */
#include "modulos/entidades/samurai/samurai.h"
#include "modulos/entidades/zumbis/zumbis.h"
#include "modulos/interface/game_over/game_over.h"
#include "modulos/interface/interface.h"
#include "modulos/interface/particulas/particulas.h"
#include "modulos/interface/ranking/ranking.h"
#include "modulos/mapa/mapa.h"
#include "modulos/sistemas/colisao/colisao.h"
#include "modulos/sistemas/sanidade/sanidade.h"
#include "modulos/sistemas/sons/sons.h"
#include "modulos/tipos.h"

/* ===== [CUTSCENE] ================================================== */
#include "modulos/cutciene/cutciene.h"

/* ===== [SELEÇÃO DE FASE + PROGRESSO] =============================== */
#include "modulos/interface/selecao_fase/selecao_fase.h"
/* =================================================================== */

/* ================================================================== */
/*  Globais de colisao                                                 */
/* ================================================================== */
unsigned char colisao[ALTURA][3000];
static ALLEGRO_BITMAP *jog_mapa_ptr = NULL;
ALLEGRO_BITMAP *get_jog_mapa(void) { return jog_mapa_ptr; }

/* ================================================================== */
/*  PROTOTIPOS                                                         */
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
void pocao_tentar_spawn(Pocao *p, VidaStatus *vidas, int mapa_id);
void pocao_atualizar(Pocao *p, Jogador *jog, VidaStatus *vidas, Sons *sons);
void pocao_desenhar(Pocao *p, ALLEGRO_BITMAP *spr);
bool pixel_solido(ALLEGRO_BITMAP *mapa, int x, int y);
bool colide_mapa(ALLEGRO_BITMAP *mapa, float x, float y);
void horda_atualizar_movimento(Horda *h, Jogador *j, Sons *sons,
                               ALLEGRO_BITMAP *mapa, const ConfigMapa *cfg);
void projetil_acido_spawn(float x, float y, int direcao);
void projeteis_acido_atualizar(ALLEGRO_BITMAP *mapa, Jogador *jog,
                               VidaStatus *vidas, Sons *sons);
void projeteis_acido_desenhar(ALLEGRO_BITMAP *spr);
void explosao_acida_spawn(float x, float y);
void explosoes_acidas_atualizar(Horda *h, Jogador *jog, VidaStatus *vidas,
                                Sanidade *san, Sons *sons);
void explosoes_acidas_desenhar(ALLEGRO_BITMAP *frames[]);

/* ================================================================== */
/*  Helpers para sliders de volume                                      */
/* ================================================================== */
static int mouse_sobre_slider(float mx, float my, float sx, float sy) {
  float margem = SLIDER_BOLA_RAIO + 4.0f;
  return (mx >= sx && mx <= sx + SLIDER_LARGURA && my >= sy - margem &&
          my <= sy + SLIDER_ALTURA + margem);
}

static float slider_valor_de_x(float mx, float sx) {
  float v = (mx - sx) / SLIDER_LARGURA;
  if (v < 0.0f)
    v = 0.0f;
  if (v > 1.0f)
    v = 1.0f;
  return v;
}

static int mouse_sobre_icone_audio(float mx, float my, float ix, float iy) {
  return (mx >= ix && mx <= ix + ICONE_AUDIO_TAM && my >= iy &&
          my <= iy + ICONE_AUDIO_TAM);
}

/* ================================================================== */
/*  Carrega bg e colisao de um mapa; libera os anteriores se existirem */
/* ================================================================== */
static void carregar_cenario(const ConfigMapa *cfg, ALLEGRO_BITMAP **bg_out,
                             ALLEGRO_BITMAP **mapa_out) {
  if (*bg_out)
    al_destroy_bitmap(*bg_out);
  if (*mapa_out)
    al_destroy_bitmap(*mapa_out);

  *bg_out = al_load_bitmap(cfg->arquivo_visual);
  *mapa_out = al_load_bitmap(cfg->arquivo_colisao);

  if (!*bg_out || !*mapa_out) {
    printf("Erro ao carregar cenario %d (%s)\n", cfg->id, cfg->nome);
  }
}

/* ================================================================== */
/*  Reseta o estado de "jogo em andamento" para iniciar uma nova fase  */
/* ================================================================== */
static void preparar_nova_fase(Jogador *jogador, Horda *horda, Pocao *pocao,
                               double *pocao_ultimo_check, Temporizador *tempo,
                               const ConfigMapa *cfg) {
  jogador->mov.x = cfg->jogador_spawn_x;
  jogador->mov.y = cfg->jogador_spawn_y;
  jogador->mov.vel_y = 0.0f;
  jogador->estado = SAM_IDLE;
  jogador->atacando = 0;
  jogador->dash_ativo = 0;
  jogador->dash_fuga_ativo = 0;
  jogador->carregando_atk2 = 0;
  jogador->carga_atk2_pct = 0.0f;
  jogador->frame_ataque = 0;
  jogador->frame = 0;
  jogador->hurt_inicio = -999.0;
  jogador->ultimo_ataque = -999.0;
  jogador->dash_fuga_ultimo = -999.0;

  horda_init(horda);
  for (int iz = 0; iz < MAX_ZUMBIS_TELA; iz++)
    horda->pool[iz].ja_stunado_no_dash = 0;
  for (int ip = 0; ip < MAX_PROJETEIS_ACIDO; ip++)
    g_projeteis_acido[ip].ativo = 0;
  for (int ie = 0; ie < MAX_EXPLOSOES_ACIDAS; ie++)
    g_explosoes_acidas[ie].ativo = 0;

  *pocao = (Pocao){0};
  *pocao_ultimo_check = 0.0;

  *tempo = (Temporizador){0};
  tempo->inicio = al_get_time();
  tempo->ativo = 1;
  carregar_ranking(tempo);
}

/* ================================================================== */
/*  Reseta TODAS as vidas para o estado inicial                       */
/* ================================================================== */
static void restaurar_todas_vidas(VidaStatus *vetor_vidas) {
  for (int i = 0; i < MAX_VIDAS; i++) {
    vetor_vidas[i].ativa = 1;
    strcpy(vetor_vidas[i].status, "Inteira");
  }
}

/* ================================================================== */
/*  MAIN                                                                */
/* ================================================================== */
int main(void) {
  srand((unsigned)time(NULL));

  if (!al_init()) {
    puts("Erro al_init");
    return 1;
  }
  if (!al_init_primitives_addon()) {
    puts("Erro primitives");
    return 1;
  }
  if (!al_install_keyboard()) {
    puts("Erro keyboard");
    return 1;
  }
  if (!al_install_mouse()) {
    puts("Erro al_install_mouse");
    return 1;
  }
  if (!al_init_image_addon()) {
    puts("Erro image");
    return 1;
  }
  al_init_font_addon();
  al_init_ttf_addon();
  if (!al_install_audio()) {
    puts("Erro al_install_audio");
    return 1;
  }
  if (!al_init_acodec_addon()) {
    puts("Erro al_init_acodec");
    return 1;
  }
  if (!al_reserve_samples(32)) {
    puts("Erro al_reserve_samples");
    return 1;
  }

  ALLEGRO_DISPLAY *display = al_create_display(LARGURA, ALTURA);
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
  if (!display || !queue || !timer) {
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
  al_register_event_source(queue, al_get_mouse_event_source());

  Sons sons = carregar_sons();
  g_sons_ativo = &sons;

  ALLEGRO_BITMAP *bg_menu = al_load_bitmap("assets/cenarios/inicio.png");
  if (!bg_menu) {
    puts("Erro bg_menu");
    return 1;
  }

  al_start_timer(timer);
  tocar_musica_fundo(&sons);

  /* ================================================================
     PROGRESSO PERSISTENTE
     ================================================================ */
  ProgressoJogo prog;
  progresso_carregar(&prog);

  /* ================================================================
     LOOP EXTERNO: Menu -> Seleção de fase -> Jogo -> (repete)
     ================================================================ */
  int rodando_global = 1;

  while (rodando_global) {
    /* ---- Menu principal ---- */
    OpcaoMenu escolha =
        executar_menu(queue, timer, bg_menu, fonte, fonte_hud, &sons);
    if (escolha == MENU_SAIR)
      break;

    al_flush_event_queue(queue);

    /* ---- Tela de seleção de fases ---- */
    int fase_escolhida =
        tela_selecao_fase(queue, timer, fonte, fonte_hud, &prog, &sons);
    if (fase_escolhida < 0)
      continue; /* volta ao menu principal */

    al_flush_event_queue(queue);

    /* ===== [CUTSCENE] – só exibe antes da fase 0 =============== */
    if (fase_escolhida == 0) {
      parar_musica_fundo(&sons);

      Cutscene cutscene;
      cutscene_criar(&cutscene);

      bool em_cutscene = true;
      bool redraw_cut = false;

      while (em_cutscene) {
        ALLEGRO_EVENT ev_cut;
        al_wait_for_event(queue, &ev_cut);

        if (ev_cut.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          cutscene_destruir(&cutscene);
          rodando_global = 0;
          em_cutscene = false;
          continue;
        }
        if (ev_cut.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
            ev_cut.mouse.button == 1)
          cutscene_mouse_click_pos(&cutscene, ev_cut.mouse.x, ev_cut.mouse.y);
        if (ev_cut.type == ALLEGRO_EVENT_KEY_DOWN &&
            ev_cut.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
          cutscene_skip(&cutscene);
        if (ev_cut.type == ALLEGRO_EVENT_TIMER) {
          cutscene_update(&cutscene, 1.0f / FPS);
          redraw_cut = true;
          if (cutscene_finalizada(&cutscene))
            em_cutscene = false;
        }
        if (redraw_cut && al_is_event_queue_empty(queue)) {
          cutscene_draw(&cutscene);
          al_flip_display();
          redraw_cut = false;
        }
      }

      cutscene_destruir(&cutscene);
      if (!rodando_global)
        break;
      al_flush_event_queue(queue);
      tocar_musica_fundo(&sons);
    }
    /* ===== [/CUTSCENE] ========================================== */

    /* ----------------------------------------------------------------
       Inicializa cenário da fase escolhida
    ---------------------------------------------------------------- */
    int mapa_idx = fase_escolhida;
    const ConfigMapa *cfg = mapa_obter(mapa_idx);

    ALLEGRO_BITMAP *bg = NULL;
    ALLEGRO_BITMAP *mapa = NULL;
    carregar_cenario(cfg, &bg, &mapa);
    gerar_mapa_colisao(mapa);
    jog_mapa_ptr = mapa;

    /* ---- Música da fase ---- */
    if (mapa_idx == 3) {
      parar_musica_fundo(&sons);
      tocar_musica_boss(&sons);
    } else {
      parar_musica_boss(&sons);
      tocar_musica_fundo(&sons);
    }

    /* ---- sprites (inalterados) --------------------------------- */
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

    ZumbiAcidoSprites zum_acido_spr;
    zum_acido_spr.walk = al_load_bitmap("assets/sprites/Zumbi-acido/Walk.png");
    zum_acido_spr.attack =
        al_load_bitmap("assets/sprites/Zumbi-acido/Attack.png");
    zum_acido_spr.hurt = al_load_bitmap("assets/sprites/Zumbi-acido/Hurt.png");
    zum_acido_spr.dead = al_load_bitmap("assets/sprites/Zumbi-acido/Dead.png");
    zum_acido_spr.idle = al_load_bitmap("assets/sprites/Zumbi-acido/Idle.png");

    ALLEGRO_BITMAP *spr_acido_projetil =
        al_load_bitmap("assets/itens/acido.png");

    ALLEGRO_BITMAP *explosao_acida_frames[FRAMES_EXPLOSAO_ACIDA];
    for (int i = 0; i < FRAMES_EXPLOSAO_ACIDA; i++) {
      char path[64];
      sprintf(path, "assets/itens/explosao_acida/Explosion_%d.png", i + 1);
      explosao_acida_frames[i] = al_load_bitmap(path);
    }

    ALLEGRO_BITMAP *coracao = al_load_bitmap("assets/itens/vida.png");
    ALLEGRO_BITMAP *spr_estamina = al_load_bitmap("assets/itens/estamina.png");
    ALLEGRO_BITMAP *spr_sanidade = al_load_bitmap("assets/itens/sanidade.png");
    ALLEGRO_BITMAP *habilidade1 =
        al_load_bitmap("assets/itens/Habilidade1.png");
    ALLEGRO_BITMAP *habilidade2 =
        al_load_bitmap("assets/itens/Habilidade2.png");
    ALLEGRO_BITMAP *habilidade3 =
        al_load_bitmap("assets/itens/Habilidade3.png");
    ALLEGRO_BITMAP *spr_pocao = al_load_bitmap("assets/itens/pocao.png");

    if (!bg || !mapa || !sam_spr.idle || !sam_spr.run || !sam_spr.jump ||
        !sam_spr.attack1 || !sam_spr.attack2 || !sam_spr.attack3 ||
        !sam_spr.hurt || !sam_spr.dead || !zum_spr.walk || !zum_spr.run ||
        !zum_spr.attack1 || !zum_spr.attack2 || !zum_spr.attack3 ||
        !zum_spr.bite || !zum_spr.hurt || !zum_spr.dead || !zum_spr.idle ||
        !zum_acido_spr.walk || !zum_acido_spr.attack || !zum_acido_spr.hurt ||
        !zum_acido_spr.dead || !zum_acido_spr.idle || !spr_acido_projetil ||
        !coracao || !spr_estamina || !habilidade1 || !habilidade2 ||
        !habilidade3) {
      puts("Erro ao carregar bitmaps");
      rodando_global = 0;
      break;
    }
    if (!spr_pocao)
      puts("Aviso: assets/itens/pocao.png nao encontrado");

    /* ---- estado do jogo --------------------------------------- */
    Jogador jogador = {0};
    jogador.mov.x = cfg->jogador_spawn_x;
    jogador.mov.y = cfg->jogador_spawn_y;
    jogador.tipo_ataque = 1;
    jogador.estamina = MAX_ESTAMINA;
    jogador.estado = SAM_IDLE;
    jogador.hurt_inicio = -999.0;
    jogador.ultimo_ataque = -999.0;
    jogador.dash_fuga_ultimo = -999.0;

    /* ---- [BOSS] Inicialização ---------------------------------- */
    Boss boss = {0};
    BossSprites boss_spr = {0};
    int boss_ativo_nessa_fase = (mapa_idx == 3);

    if (boss_ativo_nessa_fase) {
      if (!boss_sprites_carregar(&boss_spr)) {
        puts("Aviso: falha ao carregar sprites do boss");
        boss_ativo_nessa_fase = 0;
      } else {
        boss_init(&boss, 1350.0f, 100.0f);
      }
    }
    /* ---- [/BOSS] ----------------------------------------------- */

    Sanidade sanidade = {MAX_SANIDADE, 0, 0, 0, 0.0, 0};
    sanidade.ultimo_abate = al_get_time();

    Horda horda;
    horda_init(&horda);
    for (int iz = 0; iz < MAX_ZUMBIS_TELA; iz++)
      horda.pool[iz].ja_stunado_no_dash = 0;

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
    restaurar_todas_vidas(vetor_vidas);

    int lm = ALTURA / 100 + 1, cm = LARGURA / 100 + 1;
    Particula *matriz_dec = criar_matriz_decorativa(lm, cm);

    int pausado = 0;
    double pausa_inicio = 0;
    int roda_aberta = 0, roda_selecao = 1;
    sanidade.ultimo_abate = al_get_time();

    int esc_ant = 0, k_ant = 0, j_ant = 0, q_ant = 0, e_ant = 0;
    int som_finish_tocado = 0, som_gameover_tocado = 0;
    int rodando = 1;

    float mouse_x = 0.0f, mouse_y = 0.0f;
    int mouse_btn1_pressionado = 0;
    int arrastando_slider_sfx = 0;
    int arrastando_slider_musica = 0;

    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE state;

    /* ============================================================
       LOOP PRINCIPAL (desta sessão / conjunto de fases)
       ============================================================ */
    while (rodando) {
      al_wait_for_event(queue, &ev);
      if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        rodando = 0;
        rodando_global = 0;
        break;
      }

      if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
        mouse_x = (float)ev.mouse.x;
        mouse_y = (float)ev.mouse.y;
        if (pausado && mouse_btn1_pressionado) {
          if (arrastando_slider_sfx)
            sons.volume_sfx = slider_valor_de_x(mouse_x, SLIDER_SFX_X);
          if (arrastando_slider_musica) {
            sons.volume_musica = slider_valor_de_x(mouse_x, SLIDER_MUSICA_X);
            aplicar_volume_musica(&sons);
          }
        }
      }

      if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1) {
        mouse_btn1_pressionado = 1;
        float mx = (float)ev.mouse.x, my = (float)ev.mouse.y;
        if (mouse_sobre_icone_audio(mx, my, ICONE_SFX_X, ICONE_SFX_Y))
          sons.mudo_sfx = !sons.mudo_sfx;
        else if (mouse_sobre_icone_audio(mx, my, ICONE_MUSICA_X,
                                         ICONE_MUSICA_Y)) {
          sons.mudo_musica = !sons.mudo_musica;
          aplicar_volume_musica(&sons);
        }
        if (pausado) {
          if (mouse_sobre_slider(mx, my, SLIDER_SFX_X, SLIDER_SFX_Y)) {
            arrastando_slider_sfx = 1;
            sons.volume_sfx = slider_valor_de_x(mx, SLIDER_SFX_X);
          } else if (mouse_sobre_slider(mx, my, SLIDER_MUSICA_X,
                                        SLIDER_MUSICA_Y)) {
            arrastando_slider_musica = 1;
            sons.volume_musica = slider_valor_de_x(mx, SLIDER_MUSICA_X);
            aplicar_volume_musica(&sons);
          }
        }
      }

      if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && ev.mouse.button == 1) {
        mouse_btn1_pressionado = 0;
        arrastando_slider_sfx = 0;
        arrastando_slider_musica = 0;
      }

      if (ev.type != ALLEGRO_EVENT_TIMER)
        continue;

      al_get_keyboard_state(&state);

      int esc_now = al_key_down(&state, ALLEGRO_KEY_ESCAPE);
      if (esc_now && !esc_ant && !sanidade.game_over && !horda.fase_concluida &&
          jogador.estado != SAM_DEAD) {
        pausado = !pausado;
        if (pausado) {
          pausa_inicio = al_get_time();
          tocar(sons.esc_som);
          arrastando_slider_sfx = 0;
          arrastando_slider_musica = 0;
        } else {
          double dt = al_get_time() - pausa_inicio;
          tempo.inicio += dt;
          jogador.ultimo_dano += dt;
          sanidade.ultimo_abate += dt;
        }
      }
      esc_ant = esc_now;

      int morte_pronta =
          (jogador.estado == SAM_DEAD && !jogador.morte_animando);
      int em_game_over = sanidade.game_over || morte_pronta;

      if (!pausado && !em_game_over && !horda.fase_concluida) {
        atualizar_sanidade(&sanidade);
        jogador.frame += 0.15f;
        jogador.movendo = 0;
        if (tempo.ativo)
          tempo.atual = al_get_time() - tempo.inicio;

        int k_now = al_key_down(&state, ALLEGRO_KEY_K);
        if (k_now && !k_ant) {
          roda_aberta = 1;
          roda_selecao = jogador.tipo_ataque;
        }
        if (!k_now && k_ant) {
          jogador.tipo_ataque = roda_selecao;
          roda_aberta = 0;
        }
        if (roda_aberta) {
          int q_now2 = al_key_down(&state, ALLEGRO_KEY_Q);
          int e_now2 = al_key_down(&state, ALLEGRO_KEY_E);
          if (q_now2 && !q_ant) {
            roda_selecao--;
            if (roda_selecao < 1)
              roda_selecao = 3;
          }
          if (e_now2 && !e_ant) {
            roda_selecao++;
            if (roda_selecao > 3)
              roda_selecao = 1;
          }
          q_ant = q_now2;
          e_ant = e_now2;
        }
        k_ant = k_now;

        int e_now_dash = al_key_down(&state, ALLEGRO_KEY_E);
        if (!roda_aberta) {
          if (e_now_dash && !e_ant && !jogador.dash_fuga_ativo &&
              jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
              jogador.estado != SAM_ATTACK &&
              jogador.estamina >= DASH_FUGA_CUSTO &&
              (al_get_time() - jogador.dash_fuga_ultimo) >= DASH_FUGA_DELAY) {
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

        if (jogador.dash_fuga_ativo) {
          float passo_fuga = 14.0f;
          if (jogador.dash_fuga_dist > 0.0f) {
            float move;
            if (jogador.dash_fuga_dist < passo_fuga)
              move = jogador.dash_fuga_dist;
            else
              move = passo_fuga;

            float dx_fuga;
            if (jogador.direcao == 0)
              dx_fuga = move;
            else
              dx_fuga = -move;

            float nx_f = jogador.mov.x + dx_fuga;
            if (!colide_mapa(mapa, nx_f, jogador.mov.y))
              jogador.mov.x = nx_f;
            jogador.dash_fuga_dist -= move;
            double agora_dash = al_get_time();
            for (int iz = 0; iz < MAX_ZUMBIS_TELA; iz++) {
              Inimigo *z = &horda.pool[iz];
              if (!z->vivo || z->estado == ZUM_DEAD || z->stunado)
                continue;
              float dist_x =
                  fabsf((z->x + ZUMBI_HBX_OFFSET_X + ZUMBI_HBX_W / 2.0f) -
                        (jogador.mov.x + HITBOX_W / 2.0f));
              float dist_y =
                  fabsf((z->y + ZUMBI_HBX_OFFSET_Y + ZUMBI_HBX_H / 2.0f) -
                        (jogador.mov.y + HITBOX_H / 2.0f));
              if (dist_x < 70.0f && dist_y < 70.0f) {
                float kb_dir;
                if (z->x > jogador.mov.x)
                  kb_dir = 1.0f;
                else
                  kb_dir = -1.0f;
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
          }
          if (jogador.dash_fuga_dist <= 0.0f) {
            jogador.dash_fuga_ativo = 0;
            jogador.estado = SAM_IDLE;
          }
        }

        int j_now = al_key_down(&state, ALLEGRO_KEY_J);
        double agora_atk = al_get_time();
        double delay_atual;
        if (jogador.tipo_ataque == 3)
          delay_atual = DELAY_ATAQUE_3;
        else
          delay_atual = DELAY_ATAQUE_12;

        if (jogador.tipo_ataque == 2 && j_now && !j_ant && !jogador.atacando &&
            !roda_aberta && !jogador.dash_fuga_ativo &&
            !jogador.carregando_atk2 && jogador.estado != SAM_HURT &&
            jogador.estado != SAM_DEAD && jogador.estamina >= CUSTO_ATK2_MIN &&
            (agora_atk - jogador.ultimo_ataque) >= delay_atual) {
          jogador.carregando_atk2 = 1;
          jogador.carga_atk2_inicio = agora_atk;
          jogador.carga_atk2_pct = 0.0f;
          jogador.estado = SAM_CHARGING;
        }

        if (jogador.carregando_atk2) {
          double carregando_ha = agora_atk - jogador.carga_atk2_inicio;
          float pct = (float)(carregando_ha / CARGA_ATK2_TEMPO_MAX);
          if (pct > 1.0f)
            pct = 1.0f;
          jogador.carga_atk2_pct = pct;

          float custo_projetado =
              CUSTO_ATK2_MIN + (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN) * pct;
          int estamina_insuficiente = (jogador.estamina < custo_projetado);
          int soltou = (!j_now && j_ant);

          if (soltou || estamina_insuficiente) {
            if (estamina_insuficiente) {
              float pct_max = (jogador.estamina - CUSTO_ATK2_MIN) /
                              (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN);
              if (pct_max < 0.0f)
                pct_max = 0.0f;
              if (pct_max > 1.0f)
                pct_max = 1.0f;
              pct = pct_max;
            }
            float custo_final =
                CUSTO_ATK2_MIN + (CUSTO_ATK2_MAX - CUSTO_ATK2_MIN) * pct;
            int dano_final =
                DANO_ATK2_MIN +
                (int)((DANO_ATK2_MAX - DANO_ATK2_MIN) * pct + 0.5f);
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

        if (jogador.tipo_ataque != 2 && j_now && !j_ant && !jogador.atacando &&
            !roda_aberta && !jogador.dash_fuga_ativo &&
            jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
            (agora_atk - jogador.ultimo_ataque) >= delay_atual) {
          int custo_est;
          if (jogador.tipo_ataque == 3)
            custo_est = (int)CUSTO_ATK3;
          else
            custo_est = 1;
          if (jogador.estamina >= custo_est) {
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
            if (jogador.tipo_ataque == 3) {
              tocar(sons.katana_attack3);
              jogador.dash_ativo = 1;
              jogador.dash_dist = DASH_ATK3_DIST;
              jogador.dash_som_tocado = 1;
              tocar(sons.dash);
            } else
              tocar(sons.katana12);
          }
        }
        j_ant = j_now;

        if (jogador.dash_ativo && jogador.atacando) {
          float passo = 8.0f;
          if (jogador.dash_dist > 0.0f) {
            float move;
            if (jogador.dash_dist < passo)
              move = jogador.dash_dist;
            else
              move = passo;

            float dx_dash;
            if (jogador.direcao == 0)
              dx_dash = move;
            else
              dx_dash = -move;

            float nx_d = jogador.mov.x + dx_dash;
            if (!colide_mapa(mapa, nx_d, jogador.mov.y))
              jogador.mov.x = nx_d;
            jogador.dash_dist -= move;
          }
          if (jogador.dash_dist <= 0.0f || !jogador.atacando)
            jogador.dash_ativo = 0;
        }

        if (jogador.estado != SAM_HURT && jogador.estado != SAM_DEAD &&
            jogador.estado != SAM_CHARGING && !jogador.dash_fuga_ativo) {
          float nx = jogador.mov.x;
          if (al_key_down(&state, ALLEGRO_KEY_D) && !jogador.atacando &&
              !roda_aberta) {
            nx += VELOCIDADE;
            jogador.direcao = 0;
            jogador.movendo = 1;
          }
          if (al_key_down(&state, ALLEGRO_KEY_A) && !jogador.atacando &&
              !roda_aberta) {
            nx -= VELOCIDADE;
            jogador.direcao = ALLEGRO_FLIP_HORIZONTAL;
            jogador.movendo = 1;
          }
          if (!colide_mapa(mapa, nx, jogador.mov.y))
            jogador.mov.x = nx;

          if (al_key_down(&state, ALLEGRO_KEY_W) && jogador.no_chao &&
              !jogador.atacando && !roda_aberta &&
              jogador.estamina >= CUSTO_PULO) {
            jogador.mov.vel_y = FORCA_PULO;
            jogador.estamina -= CUSTO_PULO;
            if (jogador.estamina < 0)
              jogador.estamina = 0;
            tocar(sons.pulo);
          }

          if (jogador.no_chao && jogador.movendo && jogador.estado == SAM_RUN) {
            double agora_sam = al_get_time();
            if ((agora_sam - jogador.ultimo_passo_sam) >=
                SOM_WALK_SAM_INTERVALO) {
              jogador.ultimo_passo_sam = agora_sam;
              tocar(sons.walk_sam);
            }
          }
        }

        jogador.mov.vel_y += GRAVIDADE;
        if (jogador.mov.vel_y > MAX_QUEDA)
          jogador.mov.vel_y = MAX_QUEDA;
        float ny = jogador.mov.y + jogador.mov.vel_y;
        if (!colide_mapa(mapa, jogador.mov.x, ny))
          jogador.mov.y = ny;
        else
          jogador.mov.vel_y = 0;
        jogador.no_chao = esta_no_chao(mapa, jogador.mov.x, jogador.mov.y);
        if (jogador.no_chao) {
          jogador.y_chao = jogador.mov.y;
          jogador.estamina += RECARGA_ESTAMINA;
          if (jogador.estamina > MAX_ESTAMINA)
            jogador.estamina = MAX_ESTAMINA;
        }
        if (jogador.mov.y > ALTURA + 200) {
          tocar(sons.caindo);
          perder_vida(vetor_vidas);
          jogador.mov.x = cfg->jogador_spawn_x;
          jogador.mov.y = cfg->jogador_spawn_y;
          jogador.mov.vel_y = 0;
        }

        atualizar_estado_samurai(&jogador);
        if (mapa_idx != mapa_total() - 1)
          horda_atualizar_spawn(&horda, &sons, jogador.mov.y, cfg);
        horda_atualizar_fisica(&horda, mapa);
        horda_atualizar_movimento(&horda, &jogador, &sons, mapa, cfg);
        horda_verificar_ataque(&horda, &jogador, &sanidade, &sons, mapa);
        horda_verificar_dano_jogador(&horda, &jogador, vetor_vidas, &sons);
        projeteis_acido_atualizar(mapa, &jogador, vetor_vidas, &sons);
        explosoes_acidas_atualizar(&horda, &jogador, vetor_vidas, &sanidade,
                                   &sons);

        /* ---- [BOSS] Lógica --------------------------------- */
        if (boss_ativo_nessa_fase && boss.ativo) {
          boss_atualizar(&boss, &jogador, &sons, mapa, vetor_vidas);

          /* Dano do samurai no boss */
          if (jogador.atacando && !jogador.dash_fuga_ativo) {
            float bx1 = boss.x + BOSS_HBX_OFFSET_X;
            float bx2 = bx1 + BOSS_HBX_W;
            float by1 = boss.y + BOSS_HBX_OFFSET_Y;
            float by2 = by1 + BOSS_HBX_H;
            float jx1 = jogador.mov.x + HITBOX_OFFSET_X;
            float jx2 = jx1 + HITBOX_W + 60.0f;
            float jy1 = jogador.mov.y + HITBOX_OFFSET_Y;
            float jy2 = jy1 + HITBOX_H;
            if (bx1 < jx2 && bx2 > jx1 && by1 < jy2 && by2 > jy1) {
              int dano_sam;
              if (jogador.tipo_ataque == 2)
                dano_sam = jogador.dano_atk2_atual;
              else if (jogador.tipo_ataque == 3)
                dano_sam = DANO_ATK3;
              else
                dano_sam = DANO_ATK1;
              boss_receber_dano(&boss, dano_sam, &sons);
            }

          } /* Boss derrotado → conclui fase */
          if (boss_derrotado(&boss) && !horda.fase_concluida) {
            horda.fase_concluida = 1;
            if (tempo.ativo) {
              tempo.fim = al_get_time();
              tempo.atual = tempo.fim - tempo.inicio;
              tempo.ativo = 0;
            }
          }
        }
        /* ---- [/BOSS] --------------------------------------- */

        {
          double agora_poc = al_get_time();
          if (!pocao.ativa && (agora_poc - pocao_ultimo_check) >= 3.0) {
            pocao_ultimo_check = agora_poc;
            pocao_tentar_spawn(&pocao, vetor_vidas, mapa_idx);
          }
          pocao_atualizar(&pocao, &jogador, vetor_vidas, &sons);
        }

        {
          int vivas = contar_vidas(vetor_vidas);
          if (vivas == 0 && jogador.estado != SAM_DEAD) {
            jogador.estado = SAM_DEAD;
            jogador.frame_dead = 0;
            jogador.morte_animando = 1;
            jogador.mov.vel_y = -3.0f;
            jogador.som_morrendo_tocado = 0;
          }
        }
        if (jogador.estado == SAM_DEAD && jogador.morte_animando &&
            !jogador.som_morrendo_tocado) {
          tocar(sons.morrendo);
          jogador.som_morrendo_tocado = 1;
        }
        if (horda.fase_concluida && tempo.ativo) {
          tempo.fim = al_get_time();
          tempo.atual = tempo.fim - tempo.inicio;
          tempo.ativo = 0;
        }
      }

      if (!pausado && jogador.estado == SAM_DEAD && jogador.morte_animando &&
          !sanidade.game_over) {
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

      double t_agora = al_get_time();
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_bitmap(bg, 0, 0, 0);
      desenhar_matriz_fundo(matriz_dec, lm * cm, t_agora);
      pocao_desenhar(&pocao, spr_pocao);
      horda_desenhar(&horda, &zum_spr, &zum_acido_spr);
      projeteis_acido_desenhar(spr_acido_projetil);
      explosoes_acidas_desenhar(explosao_acida_frames);
      desenhar_samurai(&jogador, &sam_spr);

      /* ---- [BOSS] Desenho ------------------------------------ */
      if (boss_ativo_nessa_fase && boss.ativo) {
        boss_desenhar(&boss, &boss_spr);
        boss_desenhar_hud(&boss, fonte_hud);
      }
      /* ---- [/BOSS] ------------------------------------------- */

      desenhar_overlay_sanidade(&sanidade);

      char txt[60];
      sprintf(txt, "Tempo: %.2f s", tempo.atual);
      desenhar_hud_tempo(fonte_hud, txt);
      desenhar_hud_horda(fonte_hud, &horda);
      desenhar_vidas(vetor_vidas, coracao);
      desenhar_estamina(jogador.estamina, spr_estamina);
      desenhar_sanidade(&sanidade, spr_sanidade);
      desenhar_aviso_sanidade(&sanidade, fonte_hud, t_agora);
      desenhar_mensagem_central_insanidade(&sanidade, fonte, fonte_hud,
                                           t_agora);
      desenhar_hud_ataque(sam_spr.attack1, sam_spr.attack2, sam_spr.attack3,
                          habilidade1, habilidade2, habilidade3,
                          jogador.tipo_ataque, fonte_hud);
      desenhar_hud_dash_fuga(&jogador, fonte_hud);
      desenhar_hud_carga_atk2(&jogador, fonte_hud);
      desenhar_icones_audio(&sons, fonte_hud, mouse_x, mouse_y);

      /* Indicador de fase atual no canto superior direito */
      {
        char txt_fase[32];
        sprintf(txt_fase, "FASE %d", mapa_idx + 1);
        al_draw_text(fonte_hud, al_map_rgba(220, 180, 60, 200), LARGURA - 200,
                     47, ALLEGRO_ALIGN_RIGHT, txt_fase);
      }

      if (pausado)
        desenhar_pausa_com_volume(&sons, fonte, fonte_hud, mouse_x, mouse_y);
      if (roda_aberta)
        desenhar_roda_habilidade(sam_spr.attack1, sam_spr.attack2,
                                 sam_spr.attack3, habilidade1, habilidade2,
                                 habilidade3, roda_selecao, fonte_hud);
      al_flip_display();

      morte_pronta = (jogador.estado == SAM_DEAD && !jogador.morte_animando);
      if (morte_pronta || sanidade.game_over) {
        if (!som_gameover_tocado) {
          parar_musica_fundo(&sons);
          parar_musica_boss(&sons);
          tocar(sons.game_over_som);
          som_gameover_tocado = 1;
        }
        al_rest(0.8);

        const char *motivo;
        if (sanidade.game_over)
          motivo = "Sua sanidade chegou ao limite...";
        else
          motivo = "Voce foi derrotado pelos zumbis!";

        int reiniciar =
            tela_game_over(queue, timer, fonte, fonte_hud, motivo, &sons);
        if (reiniciar) {
          mapa_idx = prog.ultima_fase_jogada;
          cfg = mapa_obter(mapa_idx);
          carregar_cenario(cfg, &bg, &mapa);
          gerar_mapa_colisao(mapa);
          jog_mapa_ptr = mapa;

          jogador = (Jogador){0};
          jogador.tipo_ataque = 1;
          jogador.estamina = MAX_ESTAMINA;

          sanidade = (Sanidade){MAX_SANIDADE, 0, 0, 0, 0.0, 0};
          sanidade.ultimo_abate = al_get_time();

          preparar_nova_fase(&jogador, &horda, &pocao, &pocao_ultimo_check,
                             &tempo, cfg);
          restaurar_todas_vidas(vetor_vidas);

          /* ---- [BOSS] Reiniciar após game over ----------- */
          boss_ativo_nessa_fase = (mapa_idx == 3);
          if (boss_ativo_nessa_fase) {
            if (!boss_spr.idle)
              boss_sprites_carregar(&boss_spr);
            boss_init(&boss, 1350.0f, 100.0f);
          }
          /* ---- [/BOSS] ----------------------------------- */

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
          mouse_btn1_pressionado = 0;
          arrastando_slider_sfx = 0;
          arrastando_slider_musica = 0;

          if (mapa_idx == 3) {
            parar_musica_fundo(&sons);
            tocar_musica_boss(&sons);
          } else {
            parar_musica_boss(&sons);
            tocar_musica_fundo(&sons);
          }
          al_flush_event_queue(queue);
          continue;
        }
        /* Não reiniciou: volta ao menu principal */
        rodando = 0;
        continue;
      }

      /* ============================================================
         FASE CONCLUÍDA (horda derrotada nesse mapa)
         ============================================================ */
      if (horda.fase_concluida && !sanidade.game_over) {
        if (!som_finish_tocado) {
          parar_musica_fundo(&sons);
          parar_musica_boss(&sons);
          tocar(sons.finish_som);
          som_finish_tocado = 1;
        }

        /* Flash de vitória */
        al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                 al_map_rgba(0, 120, 40, 30));
        const char *txt_vitoria;
        if (boss_ativo_nessa_fase)
          txt_vitoria = "BOSS DERROTADO!";
        else
          txt_vitoria = "50 ZUMBIS ELIMINADOS!";
        al_draw_text(fonte, al_map_rgb(255, 215, 0), LARGURA / 2.0f,
                     ALTURA / 2.0f - 60, ALLEGRO_ALIGN_CENTER, txt_vitoria);
        al_draw_text(fonte_hud, al_map_rgb(255, 255, 255), LARGURA / 2.0f,
                     ALTURA / 2.0f, ALLEGRO_ALIGN_CENTER, "Abrindo ranking...");
        al_flip_display();
        al_rest(1.5);
        
        /* ---- Salva progresso (desbloqueia próxima fase) ---- */
        progresso_avancar_fase(&prog, mapa_idx);
        /* ---- Ranking da fase ---- */
        tela_ranking(queue, timer, fonte, fonte_hud, &tempo, tempo.atual);
        int existe_proximo_mapa = (mapa_idx < mapa_total() - 1);
        if (existe_proximo_mapa) {
          int proximo_idx = mapa_idx + 1;
          const ConfigMapa *cfg_prox = mapa_obter(proximo_idx);
          tela_proxima_fase(
              queue, timer, fonte, fonte_hud, mapa_idx, proximo_idx,
              cfg_prox); 

          /* ---- [BOSS] Transição de fase ------------------ */
          /* Destroi sprites do boss ao sair da fase 4 */
          if (boss_ativo_nessa_fase) {
            boss_sprites_destruir(&boss_spr);
            boss_ativo_nessa_fase = 0;
          }
          /* Ativa boss se a nova fase for a fase do boss */
          if (proximo_idx == 3) {
            boss_ativo_nessa_fase = 1;
            boss_sprites_carregar(&boss_spr);
            boss_init(&boss, 1350.0f, 100.0f);
          }
          /* ---- [/BOSS] ----------------------------------- */

          mapa_idx = proximo_idx;
          cfg = cfg_prox;

          carregar_cenario(cfg, &bg, &mapa);
          gerar_mapa_colisao(mapa);
          jog_mapa_ptr = mapa;

          preparar_nova_fase(&jogador, &horda, &pocao, &pocao_ultimo_check,
                             &tempo, cfg);

          restaurar_todas_vidas(vetor_vidas);
          sanidade = (Sanidade){MAX_SANIDADE, 0, 0, 0, 0.0, 0};
          sanidade.ultimo_abate = al_get_time();

          som_finish_tocado = 0;
          pausado = 0;
          roda_aberta = 0;
          roda_selecao = 1;
          esc_ant = 0;
          k_ant = 0;
          j_ant = 0;
          q_ant = 0;
          e_ant = 0;
          mouse_btn1_pressionado = 0;
          arrastando_slider_sfx = 0;
          arrastando_slider_musica = 0;

          if (proximo_idx == 3) {
            parar_musica_fundo(&sons);
            tocar_musica_boss(&sons);
          } else {
            parar_musica_boss(&sons);
            tocar_musica_fundo(&sons);
          }
          al_flush_event_queue(queue);
          continue;
        }

        /* Era o último mapa: encerra essa sessão, volta ao menu */
        rodando = 0;
        continue;
      }
    } /* fim loop principal desta sessão */

    /* ---- Cleanup de bitmaps da sessão ---- */
    if (bg)
      al_destroy_bitmap(bg);
    if (mapa)
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

    /* ---- [BOSS] Cleanup final da sessão ----------------------- */
    if (boss_ativo_nessa_fase)
      boss_sprites_destruir(&boss_spr);
    /* ---- [/BOSS] ----------------------------------------------- */

    free(vetor_vidas);
    liberar_matriz(matriz_dec, lm * cm);

    if (!rodando_global)
      break;

  } /* fim loop externo (menu -> jogo -> menu) */

  /* ---- Cleanup global ---- */
  destruir_sons(&sons);
  al_destroy_bitmap(bg_menu);
  al_destroy_font(fonte_hud);
  al_destroy_font(fonte);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  al_destroy_display(display);
  return 0;
}
