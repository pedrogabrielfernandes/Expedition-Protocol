#include "cutciene.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ================================================================== */
/*  Roteiro                                                             */
/* ================================================================== */
static void _definir_roteiro(Cutscene *cs)
{
    cs->num_cenas = 6;

    /* --- CENA 1: O Projeto Expedicao --- */
    cs->cenas[0].bg_path              = "assets/cenarios/cutciene/cut1.png";
    cs->cenas[0].tem_contagem         = false;
    cs->cenas[0].tem_tela_preta_depois = false;
    cs->cenas[0].usar_som_alerta      = false;
    cs->cenas[0].num_dialogos         = 4;
    cs->cenas[0].dialogos[0] = (Dialogo){ "Cientista Chefe",
        "Depois de vinte anos de pesquisa, finalmente conseguimos.",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[0].dialogos[1] = (Dialogo){ "Cientista 2",
        "A Maquina de Espaco-Tempo esta pronta para o primeiro teste.",
        "assets/sprites/dialogo/cientista2.png" };
    cs->cenas[0].dialogos[2] = (Dialogo){ "Cientista Chefe",
        "Hoje mudaremos a historia da humanidade.",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[0].dialogos[3] = (Dialogo){ "Operador",
        "Iniciando sequencia de ativacao.",
        "assets/sprites/dialogo/operador.png" };

    /* --- CENA 2: O Primeiro Teste (tem contagem + maquina) --- */
    cs->cenas[1].bg_path              = "assets/cenarios/cutciene/cut2.png";
    cs->cenas[1].tem_contagem         = true;
    cs->cenas[1].tem_tela_preta_depois = false;
    cs->cenas[1].usar_som_alerta      = false;
    cs->cenas[1].num_dialogos         = 4;
    cs->cenas[1].dialogos[0] = (Dialogo){ "Operador",
        "Nucleo estabilizado.",
        "assets/sprites/dialogo/operador.png" };
    cs->cenas[1].dialogos[1] = (Dialogo){ "Cientista 2",
        "Leituras normais em todos os setores.",
        "assets/sprites/dialogo/cientista2.png" };
    cs->cenas[1].dialogos[2] = (Dialogo){ "Cientista Chefe",
        "Abrindo portal experimental em tres... dois... um...",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[1].dialogos[3] = (Dialogo){ "Sistema",
        "PORTAL ESTABELECIDO.",
        "assets/sprites/dialogo/sistema.png" };

    /* --- CENA 3: O Inicio do Fim (alerta.wav) --- */
    cs->cenas[2].bg_path              = "assets/cenarios/cutciene/cut3.png";
    cs->cenas[2].tem_contagem         = false;
    cs->cenas[2].tem_tela_preta_depois = false;
    cs->cenas[2].usar_som_alerta      = true;
    cs->cenas[2].num_dialogos         = 4;
    cs->cenas[2].dialogos[0] = (Dialogo){ "Sistema",
        "ALERTA GLOBAL. ALERTA GLOBAL.",
        "assets/sprites/dialogo/sistema.png" };
    cs->cenas[2].dialogos[1] = (Dialogo){ "Operador",
        "Recebemos transmissoes de emergencia em varias cidades!",
        "assets/sprites/dialogo/operador.png" };
    cs->cenas[2].dialogos[2] = (Dialogo){ "Cientista 2",
        "Isso e impossivel... as pessoas estao atacando umas as outras!",
        "assets/sprites/dialogo/cientista2.png" };
    cs->cenas[2].dialogos[3] = (Dialogo){ "Cientista Chefe",
        "Fechem todas as saidas! Algo aconteceu la fora!",
        "assets/sprites/dialogo/cientista_chefe.png" };

    /* --- CENA 4: O Devorador de Mundos --- */
    cs->cenas[3].bg_path              = "assets/cenarios/cutciene/cut4.png";
    cs->cenas[3].tem_contagem         = false;
    cs->cenas[3].tem_tela_preta_depois = false;
    cs->cenas[3].usar_som_alerta      = false;
    cs->cenas[3].num_dialogos         = 4;
    cs->cenas[3].dialogos[0] = (Dialogo){ "Operador",
        "Temos contato visual com uma anomalia de escala planetaria.",
        "assets/sprites/dialogo/operador.png" };
    cs->cenas[3].dialogos[1] = (Dialogo){ "Cientista 2",
        "Meu Deus...",
        "assets/sprites/dialogo/cientista2.png" };
    cs->cenas[3].dialogos[2] = (Dialogo){ "Cientista Chefe",
        "O que e essa coisa?",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[3].dialogos[3] = (Dialogo){ "Sistema",
        "ENTIDADE DESCONHECIDA DETECTADA. NOME: DEVORADOR DE MUNDOS.",
        "assets/sprites/dialogo/sistema.png" };

    /* --- CENA 5: A Ultima Missao (tela preta + fim-do-mundo depois) --- */
    cs->cenas[4].bg_path              = "assets/cenarios/cutciene/cut5.png";
    cs->cenas[4].tem_contagem         = false;
    cs->cenas[4].tem_tela_preta_depois = true;
    cs->cenas[4].usar_som_alerta      = false;
    cs->cenas[4].num_dialogos         = 5;
    cs->cenas[4].dialogos[0] = (Dialogo){ "Cientista Chefe",
        "Perdemos. A Terra nao sobrevivera.",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[4].dialogos[1] = (Dialogo){ "Agente de Elite",
        "Entao por que me chamou?",
        "assets/sprites/dialogo/samurai.png" };
    cs->cenas[4].dialogos[2] = (Dialogo){ "Cientista Chefe",
        "Encontramos uma dimensao alternativa onde tudo comecou.",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[4].dialogos[3] = (Dialogo){ "Cientista Chefe",
        "Va para la. Descubra a origem da infestacao e impeca o Devorador de Mundos.",
        "assets/sprites/dialogo/cientista_chefe.png" };
    cs->cenas[4].dialogos[4] = (Dialogo){ "Agente de Elite",
        "Entendido. Iniciando a Expedicao.",
        "assets/sprites/dialogo/samurai.png" };

    /* --- CENA 6: Chegada na dimensao alternativa --- */
    cs->cenas[5].bg_path              = "assets/cenarios/cutciene/cut6.png";
    cs->cenas[5].tem_contagem         = false;
    cs->cenas[5].tem_tela_preta_depois = false;
    cs->cenas[5].usar_som_alerta      = false;
    cs->cenas[5].num_dialogos         = 5;
    cs->cenas[5].dialogos[0] = (Dialogo){ "Agente de Elite",
        "A transferencia foi concluida...",
        "assets/sprites/dialogo/samurai.png" };
    cs->cenas[5].dialogos[1] = (Dialogo){ "Agente de Elite",
        "Entao esta e a dimensao alternativa.",
        "assets/sprites/dialogo/samurai.png" };
    cs->cenas[5].dialogos[2] = (Dialogo){ "Agente de Elite",
        "Os cientistas disseram que o ponto de chegada ficava abaixo da superficie.",
        "assets/sprites/dialogo/samurai.png" };
    cs->cenas[5].dialogos[3] = (Dialogo){ "Agente de Elite",
        "Se houver respostas, elas estarao la em cima.",
        "assets/sprites/dialogo/samurai.png" };
    cs->cenas[5].dialogos[4] = (Dialogo){ "[OBJETIVO]",
        "Utilize o elevador para chegar a superficie.",
        "assets/sprites/dialogo/objetivo.png" };
}

/* ================================================================== */
/*  Helpers internos                                                    */
/* ================================================================== */
static void _descarregar_bg(Cutscene *cs)
{
    if (cs->bg_atual) { al_destroy_bitmap(cs->bg_atual); cs->bg_atual = NULL; }
}

static void _descarregar_sprite(Cutscene *cs)
{
    if (cs->sprite_atual) { al_destroy_bitmap(cs->sprite_atual); cs->sprite_atual = NULL; }
}

static void _carregar_bg_cena(Cutscene *cs, int idx)
{
    _descarregar_bg(cs);
    cs->bg_atual = al_load_bitmap(cs->cenas[idx].bg_path);
    if (!cs->bg_atual)
        fprintf(stderr, "cutscene: nao carregou '%s'\n", cs->cenas[idx].bg_path);
}

static void _carregar_sprite_dialogo(Cutscene *cs, int cena, int dial)
{
    _descarregar_sprite(cs);
    const char *path = cs->cenas[cena].dialogos[dial].sprite_path;
    if (path) {
        cs->sprite_atual = al_load_bitmap(path);
        if (!cs->sprite_atual)
            fprintf(stderr, "cutscene: nao carregou sprite '%s'\n", path);
    }
}

static void _iniciar_dialogo(Cutscene *cs, int d)
{
    cs->dialogo_atual   = d;
    cs->chars_mostrados = 0.0f;
    cs->texto_completo  = false;
    cs->ultimo_som_char = al_get_time();
    _carregar_sprite_dialogo(cs, cs->cena_atual, d);
}

static void _iniciar_fase(Cutscene *cs, FaseCena fase)
{
    cs->fase        = fase;
    cs->fase_inicio = al_get_time();
}

static void _tocar_som(ALLEGRO_SAMPLE *s)
{
    if (s) al_play_sample(s, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
}

static void _parar_instancia(ALLEGRO_SAMPLE_INSTANCE **inst)
{
    if (*inst) {
        al_stop_sample_instance(*inst);
        al_destroy_sample_instance(*inst);
        *inst = NULL;
    }
}

static void _parar_maquina(Cutscene *cs)   { _parar_instancia(&cs->inst_maquina);   }
static void _parar_fim_mundo(Cutscene *cs) { _parar_instancia(&cs->inst_fim_mundo); }
static void _parar_alerta(Cutscene *cs)    { _parar_instancia(&cs->inst_alerta);    }

static void _tocar_loop(ALLEGRO_SAMPLE *s, ALLEGRO_SAMPLE_INSTANCE **inst)
{
    if (!s || *inst) return;
    *inst = al_create_sample_instance(s);
    if (*inst) {
        al_set_sample_instance_playmode(*inst, ALLEGRO_PLAYMODE_LOOP);
        al_attach_sample_instance_to_mixer(*inst, al_get_default_mixer());
        al_play_sample_instance(*inst);
    }
}

static void _iniciar_contagem(Cutscene *cs)
{
    _iniciar_fase(cs, FASE_CONTAGEM);
    cs->contagem_num      = 10;
    cs->ultimo_num_tocado = al_get_time() - CONTAGEM_INTERVALO + 1.4;
    _tocar_loop(cs->som_maquina, &cs->inst_maquina);
}

static void _iniciar_cena(Cutscene *cs, int idx)
{
    cs->cena_atual = idx;
    _carregar_bg_cena(cs, idx);
    cs->alpha = 0.0f;

    /* Som de alerta em loop para cena 3 */
    if (cs->cenas[idx].usar_som_alerta)
        _tocar_loop(cs->som_alerta, &cs->inst_alerta);

    _iniciar_fase(cs, FASE_FADE_IN);
}

/* ================================================================== */
/*  cutscene_criar                                                      */
/* ================================================================== */
bool cutscene_criar(Cutscene *cs)
{
    memset(cs, 0, sizeof(Cutscene));
    _definir_roteiro(cs);

    cs->fonte_texto    = al_load_ttf_font("assets/PublicPixel.ttf", 18, 0);
    cs->fonte_nome     = al_load_ttf_font("assets/PublicPixel.ttf", 16, 0);
    cs->fonte_contagem = al_load_ttf_font("assets/PublicPixel.ttf", 120, 0);
    if (!cs->fonte_texto) {
        cs->fonte_texto    = al_create_builtin_font();
        cs->fonte_nome     = cs->fonte_texto;
        cs->fonte_contagem = cs->fonte_texto;
    }

    cs->som_dialogo   = al_load_sample("assets/sons/cutciene/dialogo.wav");
    cs->som_maquina   = al_load_sample("assets/sons/cutciene/maquina.WAV");
    cs->som_final     = al_load_sample("assets/sons/cutciene/final_maquina.WAV");
    cs->som_fim_mundo = al_load_sample("assets/sons/cutciene/fim-do-mundo.wav");
    cs->som_alerta    = al_load_sample("assets/sons/cutciene/alerta.wav");
    cs->som_portal    = al_load_sample("assets/sons/cutciene/portal.wav");

    const char *nomes_num[] = {
        "assets/sons/cutciene/10.wav", "assets/sons/cutciene/09.wav",
        "assets/sons/cutciene/8.wav",  "assets/sons/cutciene/7.wav",
        "assets/sons/cutciene/6.wav",  "assets/sons/cutciene/5.wav",
        "assets/sons/cutciene/4.wav",  "assets/sons/cutciene/3.wav",
        "assets/sons/cutciene/2.wav",  "assets/sons/cutciene/1.wav"
    };
    for (int i = 0; i < 10; i++) {
        cs->sons_num[i] = al_load_sample(nomes_num[i]);
        if (!cs->sons_num[i])
            fprintf(stderr, "cutscene: nao carregou '%s'\n", nomes_num[i]);
    }

    _iniciar_cena(cs, 0);
    return true;
}

/* ================================================================== */
/*  cutscene_update                                                     */
/* ================================================================== */
void cutscene_update(Cutscene *cs, float dt)
{
    if (cs->finalizada) return;

    double agora   = al_get_time();
    double elapsed = agora - cs->fase_inicio;

    switch (cs->fase) {

    case FASE_FADE_IN:
        cs->alpha = (float)(elapsed / FADE_DURACAO);
        if (cs->alpha >= 1.0f) { cs->alpha = 1.0f; _iniciar_fase(cs, FASE_FUNDO); }
        break;

    case FASE_FUNDO:
        if (elapsed >= FUNDO_EXIBE_DURACAO) {
            _iniciar_fase(cs, FASE_DIALOGO);
            _iniciar_dialogo(cs, 0);
        }
        break;

    case FASE_DIALOGO: {
        CenaDef *cena = &cs->cenas[cs->cena_atual];
        Dialogo *dial = &cena->dialogos[cs->dialogo_atual];
        int      len  = (int)strlen(dial->texto);

        if (!cs->texto_completo) {
            cs->chars_mostrados += CHARS_POR_SEGUNDO * dt;
            if (cs->chars_mostrados >= len) {
                cs->chars_mostrados = (float)len;
                cs->texto_completo  = true;
            }
            int chars_int = (int)cs->chars_mostrados;
            if (chars_int > 0 && chars_int < len && (agora - cs->ultimo_som_char) > 0.05) {
                _tocar_som(cs->som_dialogo);
                cs->ultimo_som_char = agora;
            }
        }

        if (cs->clique_pendente) {
            cs->clique_pendente = false;
            if (!cs->texto_completo) {
                cs->chars_mostrados = (float)len;
                cs->texto_completo  = true;
            } else {
                int proximo = cs->dialogo_atual + 1;
                if (proximo < cena->num_dialogos) {
                    _iniciar_dialogo(cs, proximo);
                } else {
                    /* Fim dos dialogos desta cena */
                    _parar_alerta(cs); /* para alerta se estava tocando */
                    if (cena->tem_contagem) {
                        _iniciar_contagem(cs);
                    } else if (cena->tem_tela_preta_depois) {
                        _iniciar_fase(cs, FASE_TELA_PRETA);
                        _tocar_loop(cs->som_fim_mundo, &cs->inst_fim_mundo);
                    } else {
                        _iniciar_fase(cs, FASE_FADE_OUT);
                    }
                }
            }
        }
        break;
    }

    case FASE_CONTAGEM: {
        if (cs->contagem_num > 0) {
            if ((agora - cs->ultimo_num_tocado) >= CONTAGEM_INTERVALO) {
                cs->ultimo_num_tocado = agora;
                int idx = 10 - cs->contagem_num;
                _tocar_som(cs->sons_num[idx]);
                cs->contagem_num--;
            }
        } else {
            _parar_maquina(cs);
            _tocar_som(cs->som_final);
            _iniciar_fase(cs, FASE_FADE_OUT);
        }
        break;
    }

    case FASE_TELA_PRETA:
        /* Tela preta por 5 segundos com fim-do-mundo tocando */
        cs->alpha = 0.0f;
        if (elapsed >= TELA_PRETA_DURACAO) {
            _parar_fim_mundo(cs);
            _tocar_som(cs->som_portal);
            int proxima = cs->cena_atual + 1;
            if (proxima < cs->num_cenas) {
                _iniciar_cena(cs, proxima);
            } else {
                cs->fase      = FASE_FIM;
                cs->finalizada = true;
            }
        }
        break;

    case FASE_FADE_OUT:
        cs->alpha = 1.0f - (float)(elapsed / FADE_DURACAO);
        if (cs->alpha <= 0.0f) {
            cs->alpha = 0.0f;
            int proxima = cs->cena_atual + 1;
            if (proxima < cs->num_cenas) {
                _iniciar_cena(cs, proxima);
            } else {
                cs->fase      = FASE_FIM;
                cs->finalizada = true;
            }
        }
        break;

    case FASE_FIM:
        cs->finalizada = true;
        break;
    }

    cs->clique_pendente = false;
}

/* ================================================================== */
/*  cutscene_draw                                                       */
/* ================================================================== */
void cutscene_draw(Cutscene *cs)
{
    if (cs->finalizada) return;

    ALLEGRO_DISPLAY *disp = al_get_current_display();
    int sw = al_get_display_width(disp);
    int sh = al_get_display_height(disp);

    al_clear_to_color(al_map_rgb(0, 0, 0));

    /* ---- Tela preta (nao desenha nada, so o fundo preto) ---- */
    if (cs->fase == FASE_TELA_PRETA) return;

    /* ---- Background com fade ---- */
    if (cs->bg_atual) {
        int bw = al_get_bitmap_width(cs->bg_atual);
        int bh = al_get_bitmap_height(cs->bg_atual);
        float scale = (float)sw / bw;
        if (bh * scale < sh) scale = (float)sh / bh;
        float dw = bw * scale, dh = bh * scale;
        float ox = (sw - dw) / 2.0f, oy = (sh - dh) / 2.0f;
        al_draw_tinted_scaled_bitmap(cs->bg_atual,
            al_map_rgba_f(cs->alpha, cs->alpha, cs->alpha, cs->alpha),
            0, 0, bw, bh, ox, oy, dw, dh, 0);
    }

    /* ---- Contagem regressiva ---- */
    if (cs->fase == FASE_CONTAGEM && cs->contagem_num > 0) {
        ALLEGRO_FONT *fc;
        if (cs->fonte_contagem) {
            fc = cs->fonte_contagem;
        } else {
            fc = cs->fonte_texto;
        }
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", cs->contagem_num);
        int cx = sw / 2, cy = sh / 2;
        al_draw_text(fc, al_map_rgba(0,0,0,200), cx+4, cy - al_get_font_line_height(fc)/2 + 4,
                     ALLEGRO_ALIGN_CENTRE, buf);
        al_draw_text(fc, al_map_rgb(255,220,50), cx, cy - al_get_font_line_height(fc)/2,
                     ALLEGRO_ALIGN_CENTRE, buf);
        goto desenhar_botao_pular;
    }

    /* ---- Caixa de dialogo ---- */
    if (cs->fase == FASE_DIALOGO) {
        CenaDef *cena = &cs->cenas[cs->cena_atual];
        Dialogo *dial = &cena->dialogos[cs->dialogo_atual];

        int caixa_y  = (int)(sh * CAIXA_Y_FUNDO);
        int caixa_h  = CAIXA_ALTURA;
        int sprite_w = SPRITE_LARGURA;
        int sprite_h = SPRITE_ALTURA;
        int margem   = CAIXA_MARGEM;

        int sprite_x = margem;
        int sprite_y = caixa_y + (caixa_h - sprite_h) / 2;
        int caixa_x  = sprite_x + sprite_w + margem;
        int caixa_w  = sw - caixa_x - margem;
        int total_y  = caixa_y - margem;

        /* Fundo escuro */
        al_draw_filled_rectangle(0, total_y, sw, total_y + caixa_h + margem * 3,
                                 al_map_rgba(0, 0, 0, 200));

        /* Retangulo sprite */
        al_draw_filled_rounded_rectangle(sprite_x - 4, sprite_y - 4,
            sprite_x + sprite_w + 4, sprite_y + sprite_h + 4,
            8, 8, al_map_rgba(80, 120, 180, 180));
        al_draw_rounded_rectangle(sprite_x - 4, sprite_y - 4,
            sprite_x + sprite_w + 4, sprite_y + sprite_h + 4,
            8, 8, al_map_rgb(120, 180, 255), 2.0f);

        /* Sprite */
        if (cs->sprite_atual) {
            int pw = al_get_bitmap_width(cs->sprite_atual);
            int ph = al_get_bitmap_height(cs->sprite_atual);
            float sc = (float)sprite_w / pw;
            if (ph * sc > sprite_h) sc = (float)sprite_h / ph;
            int dw = (int)(pw * sc), dh = (int)(ph * sc);
            int dx = sprite_x + (sprite_w - dw) / 2;
            int dy = sprite_y + (sprite_h - dh) / 2;
            al_draw_scaled_bitmap(cs->sprite_atual, 0, 0, pw, ph, dx, dy, dw, dh, 0);
        } else {
            al_draw_filled_circle(sprite_x + sprite_w/2, sprite_y + sprite_h/2,
                                  40, al_map_rgb(60, 60, 80));
            if (cs->fonte_nome) {
                al_draw_text(cs->fonte_nome, al_map_rgb(150,150,255),
                    sprite_x + sprite_w/2, sprite_y + sprite_h/2 - 10,
                    ALLEGRO_ALIGN_CENTRE, "[SYS]");
            }
        }

        /* Caixa de fala */
        al_draw_filled_rounded_rectangle(caixa_x, caixa_y,
            caixa_x + caixa_w, caixa_y + caixa_h,
            10, 10, al_map_rgba(10, 10, 30, 220));
        al_draw_rounded_rectangle(caixa_x, caixa_y,
            caixa_x + caixa_w, caixa_y + caixa_h,
            10, 10, al_map_rgb(100, 160, 255), 2.0f);

        /* Nome */
        ALLEGRO_FONT *fn;
        if (cs->fonte_nome) {
            fn = cs->fonte_nome;
        } else {
            fn = cs->fonte_texto;
        }
        int nome_y = caixa_y + 10;
        al_draw_text(fn, al_map_rgb(100, 200, 255), caixa_x + 15, nome_y, 0, dial->personagem);

        /* Separador */
        int sep_y = nome_y + al_get_font_line_height(fn) + 6;
        al_draw_line(caixa_x + 10, sep_y, caixa_x + caixa_w - 10, sep_y,
                     al_map_rgba(100, 160, 255, 120), 1.0f);

        /* Texto digitado com quebra de linha */
        ALLEGRO_FONT *ft = cs->fonte_texto;
        int texto_y   = sep_y + 10;
        int texto_x   = caixa_x + 15;
        int texto_max = caixa_w - 30;
        int n = (int)cs->chars_mostrados;
        int len = (int)strlen(dial->texto);
        if (n > len) n = len;

        char visivel[512] = {0};
        strncpy(visivel, dial->texto, n);
        visivel[n] = '\0';

        char linha[256] = {0};
        int li = 0, lh = al_get_font_line_height(ft) + 4, ly = texto_y;
        for (int i = 0; i <= n; i++) {
            char c = visivel[i];
            if (c == '\0') {
                linha[li] = '\0';
                al_draw_text(ft, al_map_rgb(230,230,230), texto_x, ly, 0, linha);
                break;
            }
            linha[li] = c; linha[li+1] = '\0'; li++;
            if (al_get_text_width(ft, linha) >= texto_max || c == '\n') {
                int ue = li - 1;
                while (ue > 0 && linha[ue] != ' ') ue--;
                if (ue > 0 && c != '\n') {
                    linha[ue] = '\0';
                    al_draw_text(ft, al_map_rgb(230,230,230), texto_x, ly, 0, linha);
                    ly += lh;
                    char resto[256];
                    strcpy(resto, linha + ue + 1);
                    int rlen = (int)strlen(resto);
                    resto[rlen] = c; resto[rlen+1] = '\0';
                    strcpy(linha, resto); li = (int)strlen(linha);
                } else {
                    al_draw_text(ft, al_map_rgb(230,230,230), texto_x, ly, 0, linha);
                    ly += lh; li = 0; linha[0] = '\0';
                }
            }
        }

        /* Seta pisca */
        if (cs->texto_completo) {
            if (fmod(al_get_time(), 1.0) < 0.6) {
                al_draw_text(fn, al_map_rgb(150,200,255),
                    caixa_x + caixa_w - 20, caixa_y + caixa_h - 30,
                    ALLEGRO_ALIGN_RIGHT, "?");
            }
        }
    }

desenhar_botao_pular:;
    /* ---- Botao PULAR (canto inferior direito) ---- */
    ALLEGRO_FONT *fn2;
    if (cs->fonte_nome) {
        fn2 = cs->fonte_nome;
    } else {
        fn2 = cs->fonte_texto;
    }
    int bx = sw - BTN_PULAR_W - BTN_PULAR_MARGEM;
    int by = sh - BTN_PULAR_H - BTN_PULAR_MARGEM;
    al_draw_filled_rounded_rectangle(bx, by, bx + BTN_PULAR_W, by + BTN_PULAR_H,
                                     6, 6, al_map_rgba(20, 20, 60, 200));
    al_draw_rounded_rectangle(bx, by, bx + BTN_PULAR_W, by + BTN_PULAR_H,
                              6, 6, al_map_rgb(100, 160, 255), 1.5f);
    al_draw_text(fn2, al_map_rgb(180, 220, 255),
                 bx + BTN_PULAR_W / 2, by + (BTN_PULAR_H - al_get_font_line_height(fn2)) / 2,
                 ALLEGRO_ALIGN_CENTRE, "Pular");
}

/* ================================================================== */
/*  API publica                                                         */
/* ================================================================== */
void cutscene_mouse_click(Cutscene *cs)
{
    cs->clique_pendente = true;
}

void cutscene_mouse_click_pos(Cutscene *cs, int mx, int my)
{
    ALLEGRO_DISPLAY *disp = al_get_current_display();
    int sw = al_get_display_width(disp);
    int sh = al_get_display_height(disp);
    int bx = sw - BTN_PULAR_W - BTN_PULAR_MARGEM;
    int by = sh - BTN_PULAR_H - BTN_PULAR_MARGEM;

    if (mx >= bx && mx <= bx + BTN_PULAR_W &&
        my >= by && my <= by + BTN_PULAR_H)
    {
        /* Clique no botao Pular: encerra tudo imediatamente */
        _parar_maquina(cs);
        _parar_fim_mundo(cs);
        _parar_alerta(cs);
        cs->fase      = FASE_FIM;
        cs->finalizada = true;
    }
    else
    {
        cs->clique_pendente = true;
    }
}

void cutscene_skip(Cutscene *cs)
{
    if (cs->fase == FASE_FADE_IN  || cs->fase == FASE_FUNDO   ||
        cs->fase == FASE_DIALOGO  || cs->fase == FASE_CONTAGEM ||
        cs->fase == FASE_TELA_PRETA)
    {
        _parar_maquina(cs);
        _parar_fim_mundo(cs);
        _parar_alerta(cs);
        _iniciar_fase(cs, FASE_FADE_OUT);
    }
}

bool cutscene_finalizada(const Cutscene *cs) { return cs->finalizada; }

void cutscene_destruir(Cutscene *cs)
{
    _descarregar_bg(cs);
    _descarregar_sprite(cs);
    _parar_maquina(cs);
    _parar_fim_mundo(cs);
    _parar_alerta(cs);

    if (cs->som_dialogo)   al_destroy_sample(cs->som_dialogo);
    if (cs->som_maquina)   al_destroy_sample(cs->som_maquina);
    if (cs->som_final)     al_destroy_sample(cs->som_final);
    if (cs->som_fim_mundo) al_destroy_sample(cs->som_fim_mundo);
    if (cs->som_alerta)    al_destroy_sample(cs->som_alerta);
    if (cs->som_portal)    al_destroy_sample(cs->som_portal);
    for (int i = 0; i < 10; i++) {
        if (cs->sons_num[i]) al_destroy_sample(cs->sons_num[i]);
    }

    if (cs->fonte_texto && cs->fonte_texto != cs->fonte_nome)
        al_destroy_font(cs->fonte_texto);
    if (cs->fonte_nome)
        al_destroy_font(cs->fonte_nome);
    if (cs->fonte_contagem && cs->fonte_contagem != cs->fonte_nome)
        al_destroy_font(cs->fonte_contagem);
}