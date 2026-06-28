#include "sons.h"

#include <allegro5/allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Sons *g_sons_ativo = NULL;

#define COOLDOWN_SOM 0.05

typedef struct NoSomTocado
{
    ALLEGRO_SAMPLE *sample;
    double ultimo_tempo;
    struct NoSomTocado *prox;
} NoSomTocado;

static NoSomTocado *g_lista_sons_tocados = NULL;

static int pode_tocar(ALLEGRO_SAMPLE *s)
{
    double agora = al_get_time();
    NoSomTocado *atual = g_lista_sons_tocados;

    while (atual)
    {
        if (atual->sample == s)
        {
            if (agora - atual->ultimo_tempo < COOLDOWN_SOM)
                return 0;

            atual->ultimo_tempo = agora;
            return 1;
        }
        atual = atual->prox;
    }

    NoSomTocado *novo = (NoSomTocado *)malloc(sizeof(NoSomTocado));
    if (!novo)
        return 1;

    novo->sample = s;
    novo->ultimo_tempo = agora;
    novo->prox = g_lista_sons_tocados;
    g_lista_sons_tocados = novo;

    return 1;
}

static void liberar_lista_sons_tocados(void)
{
    NoSomTocado *atual = g_lista_sons_tocados;

    while (atual)
    {
        NoSomTocado *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    g_lista_sons_tocados = NULL;
}

/* ================================================================== */
/*  Helpers internos de volume                                         */
/* ================================================================== */

static float volume_sfx_efetivo(void)
{
    if (!g_sons_ativo)
        return 1.0f;
    if (g_sons_ativo->mudo_sfx)
        return 0.0f;
    return g_sons_ativo->volume_sfx;
}

static float volume_musica_efetivo(Sons *s)
{
    if (!s)
        return 1.0f;
    if (s->mudo_musica)
        return 0.0f;
    return s->volume_musica;
}

/* ================================================================== */
/*  API pública                                                        */
/* ================================================================== */

void tocar(ALLEGRO_SAMPLE *s)
{
    if (s && pode_tocar(s))
        al_play_sample(
            s,
            volume_sfx_efetivo(),
            0.0f,
            1.0f,
            ALLEGRO_PLAYMODE_ONCE,
            NULL);
}

void tocar_pitch(ALLEGRO_SAMPLE *s, float pitch)
{
    if (s && pode_tocar(s))
        al_play_sample(
            s,
            volume_sfx_efetivo(),
            0.0f,
            pitch,
            ALLEGRO_PLAYMODE_ONCE,
            NULL);
}

/* ------------------------------------------------------------------ */
/*  Música de fundo (tema padrão)                                      */
/* ------------------------------------------------------------------ */

void tocar_musica_fundo(Sons *s)
{
    if (!s->padrao_inst || s->padrao_tocando)
        return;

    al_set_sample_instance_gain(
        s->padrao_inst,
        volume_musica_efetivo(s));

    al_play_sample_instance(s->padrao_inst);

    s->padrao_tocando = 1;
}

void parar_musica_fundo(Sons *s)
{
    if (!s->padrao_inst)
        return;

    al_stop_sample_instance(s->padrao_inst);

    s->padrao_tocando = 0;
}

void aplicar_volume_musica(Sons *s)
{
    if (s->padrao_inst)
        al_set_sample_instance_gain(
            s->padrao_inst,
            volume_musica_efetivo(s));

    if (s->boss_inst)
        al_set_sample_instance_gain(
            s->boss_inst,
            volume_musica_efetivo(s));
}

/* ------------------------------------------------------------------ */
/*  Música do boss                                                     */
/* ------------------------------------------------------------------ */

void tocar_musica_boss(Sons *s)
{
    if (!s->boss_inst || s->boss_tocando)
        return;

    al_set_sample_instance_gain(
        s->boss_inst,
        volume_musica_efetivo(s));

    al_play_sample_instance(s->boss_inst);

    s->boss_tocando = 1;
}

void parar_musica_boss(Sons *s)
{
    if (!s->boss_inst)
        return;

    al_stop_sample_instance(s->boss_inst);

    s->boss_tocando = 0;
}

void aplicar_volume_musica_boss(Sons *s)
{
    if (!s->boss_inst)
        return;

    al_set_sample_instance_gain(
        s->boss_inst,
        volume_musica_efetivo(s));
}

/* ================================================================== */
/*  Carregamento                                                       */
/* ================================================================== */

Sons carregar_sons(void)
{
    Sons s;
    memset(&s, 0, sizeof(Sons));

    s.volume_sfx    = VOLUME_PADRAO;
    s.volume_musica = VOLUME_PADRAO;
    s.mudo_sfx      = 0;
    s.mudo_musica   = 0;

    /* ---- Samurai ---- */
    s.katana12       = al_load_sample("assets/sons/Samurai/Katana12.wav");
    s.katana_attack3 = al_load_sample("assets/sons/Samurai/Katana_attack3.wav");
    s.dash           = al_load_sample("assets/sons/Samurai/dash.wav");
    s.acerto_zumbi   = al_load_sample("assets/sons/Samurai/acerto_zumbi.wav");
    s.dano_sofrido   = al_load_sample("assets/sons/Samurai/dano_sofrido.wav");
    s.pulo           = al_load_sample("assets/sons/Samurai/pulo.wav");
    s.walk_sam       = al_load_sample("assets/sons/Samurai/walk.wav");
    s.morrendo       = al_load_sample("assets/sons/Samurai/morrendo.wav");
    s.caindo         = al_load_sample("assets/sons/Samurai/caindo.wav");
    s.dash_fuga      = s.dash;

    /* ---- Zumbi normal ---- */
    s.dano            = al_load_sample("assets/sons/Zumbi/dano.wav");
    s.dano_miss       = al_load_sample("assets/sons/Zumbi/dano_miss.wav");
    s.morte_zumbi     = al_load_sample("assets/sons/Zumbi/morte_zumbi.wav");
    s.dano_zumbi      = al_load_sample("assets/sons/Zumbi/dano_zumbi.wav");
    s.zumbi_knockback = al_load_sample("assets/sons/Zumbi/zumbi_KnockBack.ogg");

    /* ---- Velocista ---- */
    s.som_velocista          = al_load_sample("assets/sons/Zumbi/som_velocista.wav");
    s.ataque_velocista       = al_load_sample("assets/sons/Zumbi/ataque_velocista.wav");
    s.dano_velocista         = al_load_sample("assets/sons/Zumbi/dano_velocista.wav");
    s.morte_velocista        = al_load_sample("assets/sons/Zumbi/morte_velocista.wav");
    s.dano_sofrido_velocista = al_load_sample("assets/sons/Zumbi/dano_sofrido_velocista.wav");

    /* ---- Música de fundo (tema padrão) ---- */
    s.padrao      = al_load_sample("assets/sons/game/padrao.wav");
    s.padrao_inst = NULL;

    if (s.padrao)
    {
        s.padrao_inst = al_create_sample_instance(s.padrao);

        if (s.padrao_inst)
        {
            al_attach_sample_instance_to_mixer(
                s.padrao_inst,
                al_get_default_mixer());

            al_set_sample_instance_playmode(
                s.padrao_inst,
                ALLEGRO_PLAYMODE_LOOP);
        }
    }

    s.padrao_tocando = 0;

    /* ---- Música do boss ---- */
    s.boss_fight_music = al_load_sample("assets/sons/game/boss_fight_music.wav");
    s.boss_inst        = NULL;

    if (s.boss_fight_music)
    {
        s.boss_inst = al_create_sample_instance(s.boss_fight_music);

        if (s.boss_inst)
        {
            al_attach_sample_instance_to_mixer(
                s.boss_inst,
                al_get_default_mixer());

            al_set_sample_instance_playmode(
                s.boss_inst,
                ALLEGRO_PLAYMODE_LOOP);
        }
    }
    else
    {
        puts("Aviso: assets/sons/game/boss_fight.wav nao encontrado");
    }

    s.boss_tocando = 0;

    /* ---- Efeitos de jogo ---- */
    s.jogar         = al_load_sample("assets/sons/game/jogar.wav");
    s.esc_som       = al_load_sample("assets/sons/game/esc.wav");
    s.game_over_som = al_load_sample("assets/sons/game/game_over.wav");
    s.pocao_som     = al_load_sample("assets/sons/game/pocao.wav");
    s.sair_som      = al_load_sample("assets/sons/game/sair.wav");
    s.finish_som    = al_load_sample("assets/sons/game/finish.wav");
    s.selecao_som   = al_load_sample("assets/sons/game/selecao.wav");
    s.horda_som     = al_load_sample("assets/sons/Zumbi/horda.wav");

    /* ---- Zumbi ácido ---- */
    s.acido_impacto    = al_load_sample("assets/sons/Zumbi/acido/acido_impacto.wav");
    s.cuspindo_acido   = al_load_sample("assets/sons/Zumbi/acido/cuspindo_acido.wav");
    s.dano_zumbi_acido = al_load_sample("assets/sons/Zumbi/acido/dano_zumbi_acido.wav");
    s.explosao_acida   = al_load_sample("assets/sons/Zumbi/acido/explosao.wav");
    s.explosao_acida2  = al_load_sample("assets/sons/Zumbi/acido/explosao2.wav");
    s.morte_acido      = al_load_sample("assets/sons/Zumbi/acido/morte_acido.wav");
    s.impacto_espada   = al_load_sample("assets/sons/Zumbi/acido/impacto_espada.wav");

    /* ---- Efeitos sonoros do boss ---- */
    s.boss_spawn        = al_load_sample("assets/sons/Boss/spawn_boss.wav");
    s.boss_furioso      = al_load_sample("assets/sons/Boss/boss_furioso.wav");
    s.boss_ataque       = al_load_sample("assets/sons/Boss/som_ataque_boss.wav");
    s.boss_espada       = al_load_sample("assets/sons/Boss/espada_boss.wav");
    s.boss_ataque_errou = al_load_sample("assets/sons/Boss/errando_ataque_boss.wav");
    s.boss_morte        = al_load_sample("assets/sons/Boss/boss_morte.wav");
    s.boss_dano         = al_load_sample("assets/sons/Boss/dano_boss.wav");
    s.boss_dano_sofrido = al_load_sample("assets/sons/Boss/dano_sofrido_boss.wav");

    if (!s.boss_spawn)        puts("Aviso: assets/sons/Boss/boss_spawn.wav nao encontrado");
    if (!s.boss_furioso)      puts("Aviso: assets/sons/Boss/boss_furioso.wav nao encontrado");
    if (!s.boss_ataque)       puts("Aviso: assets/sons/Boss/boss_ataque.wav nao encontrado");
    if (!s.boss_espada)       puts("Aviso: assets/sons/Boss/boss_espada.wav nao encontrado");
    if (!s.boss_ataque_errou) puts("Aviso: assets/sons/Boss/boss_ataque_errou.wav nao encontrado");
    if (!s.boss_morte)        puts("Aviso: assets/sons/Boss/boss_morte.wav nao encontrado");
    if (!s.boss_dano)         puts("Aviso: assets/sons/Boss/boss_dano.wav nao encontrado");
    if (!s.boss_dano_sofrido) puts("Aviso: assets/sons/Boss/boss_dano_sofrido.wav nao encontrado");

    return s;
}

/* ================================================================== */
/*  Destruição                                                         */
/* ================================================================== */

void destruir_sons(Sons *s)
{
    parar_musica_fundo(s);
    parar_musica_boss(s);
    liberar_lista_sons_tocados();

    /* ---- Samurai ---- */
    if (s->katana12)       al_destroy_sample(s->katana12);
    if (s->katana_attack3) al_destroy_sample(s->katana_attack3);
    if (s->dash)           al_destroy_sample(s->dash);
    if (s->acerto_zumbi)   al_destroy_sample(s->acerto_zumbi);
    if (s->dano_sofrido)   al_destroy_sample(s->dano_sofrido);
    if (s->pulo)           al_destroy_sample(s->pulo);
    if (s->walk_sam)       al_destroy_sample(s->walk_sam);
    if (s->morrendo)       al_destroy_sample(s->morrendo);
    if (s->caindo)         al_destroy_sample(s->caindo);

    /* ---- Zumbi normal ---- */
    if (s->dano)            al_destroy_sample(s->dano);
    if (s->dano_miss)       al_destroy_sample(s->dano_miss);
    if (s->morte_zumbi)     al_destroy_sample(s->morte_zumbi);
    if (s->dano_zumbi)      al_destroy_sample(s->dano_zumbi);
    if (s->zumbi_knockback) al_destroy_sample(s->zumbi_knockback);

    /* ---- Velocista ---- */
    if (s->som_velocista)          al_destroy_sample(s->som_velocista);
    if (s->ataque_velocista)       al_destroy_sample(s->ataque_velocista);
    if (s->dano_velocista)         al_destroy_sample(s->dano_velocista);
    if (s->morte_velocista)        al_destroy_sample(s->morte_velocista);
    if (s->dano_sofrido_velocista) al_destroy_sample(s->dano_sofrido_velocista);

    /* ---- Música de fundo (tema padrão) ---- */
    if (s->padrao_inst) al_destroy_sample_instance(s->padrao_inst);
    if (s->padrao)      al_destroy_sample(s->padrao);

    /* ---- Música do boss ---- */
    if (s->boss_inst)        al_destroy_sample_instance(s->boss_inst);
    if (s->boss_fight_music) al_destroy_sample(s->boss_fight_music);

    /* ---- Efeitos de jogo ---- */
    if (s->jogar)         al_destroy_sample(s->jogar);
    if (s->esc_som)       al_destroy_sample(s->esc_som);
    if (s->game_over_som) al_destroy_sample(s->game_over_som);
    if (s->pocao_som)     al_destroy_sample(s->pocao_som);
    if (s->sair_som)      al_destroy_sample(s->sair_som);
    if (s->finish_som)    al_destroy_sample(s->finish_som);
    if (s->selecao_som)   al_destroy_sample(s->selecao_som);
    if (s->horda_som)     al_destroy_sample(s->horda_som);

    /* ---- Zumbi ácido ---- */
    if (s->acido_impacto)    al_destroy_sample(s->acido_impacto);
    if (s->cuspindo_acido)   al_destroy_sample(s->cuspindo_acido);
    if (s->dano_zumbi_acido) al_destroy_sample(s->dano_zumbi_acido);
    if (s->explosao_acida)   al_destroy_sample(s->explosao_acida);
    if (s->explosao_acida2)  al_destroy_sample(s->explosao_acida2);
    if (s->morte_acido)      al_destroy_sample(s->morte_acido);
    if (s->impacto_espada)   al_destroy_sample(s->impacto_espada);

    /* ---- Efeitos sonoros do boss ---- */
    if (s->boss_spawn)        al_destroy_sample(s->boss_spawn);
    if (s->boss_furioso)      al_destroy_sample(s->boss_furioso);
    if (s->boss_ataque)       al_destroy_sample(s->boss_ataque);
    if (s->boss_espada)       al_destroy_sample(s->boss_espada);
    if (s->boss_ataque_errou) al_destroy_sample(s->boss_ataque_errou);
    if (s->boss_morte)        al_destroy_sample(s->boss_morte);
    if (s->boss_dano)         al_destroy_sample(s->boss_dano);
    if (s->boss_dano_sofrido) al_destroy_sample(s->boss_dano_sofrido);

    if (g_sons_ativo == s)
        g_sons_ativo = NULL;
}