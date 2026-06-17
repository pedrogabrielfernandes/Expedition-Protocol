#include "sons.h"

#include <stdio.h>
#include <string.h>


void tocar(ALLEGRO_SAMPLE *s)
{
    if (s)
        al_play_sample(
            s,
            1.0f,
            0.0f,
            1.0f,
            ALLEGRO_PLAYMODE_ONCE,
            NULL
        );
}


void tocar_pitch(ALLEGRO_SAMPLE *s, float pitch)
{
    if (s)
        al_play_sample(
            s,
            1.0f,
            0.0f,
            pitch,
            ALLEGRO_PLAYMODE_ONCE,
            NULL
        );
}


void tocar_musica_fundo(Sons *s)
{
    if (!s->padrao || s->padrao_tocando)
        return;


    if (al_play_sample(
            s->padrao,
            1.0f,
            0.0f,
            1.0f,
            ALLEGRO_PLAYMODE_LOOP,
            &s->padrao_id))
    {
        s->padrao_tocando = 1;
    }
}


void parar_musica_fundo(Sons *s)
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

    memset(&s, 0, sizeof(Sons));


    /* Samurai */
    s.katana12 =
        al_load_sample("assets/sons/Samurai/Katana12.wav");

    s.katana_attack3 =
        al_load_sample("assets/sons/Samurai/Katana_attack3.wav");

    s.dash =
        al_load_sample("assets/sons/Samurai/dash.wav");

    s.acerto_zumbi =
        al_load_sample("assets/sons/Samurai/acerto_zumbi.wav");

    s.dano_sofrido =
        al_load_sample("assets/sons/Samurai/dano_sofrido.wav");

    s.pulo =
        al_load_sample("assets/sons/Samurai/pulo.wav");

    s.walk_sam =
        al_load_sample("assets/sons/Samurai/walk.wav");

    s.morrendo =
        al_load_sample("assets/sons/Samurai/morrendo.wav");

    s.caindo =
        al_load_sample("assets/sons/Samurai/caindo.wav");

    s.dash_fuga = s.dash;


    /* Zumbi */
    s.dano =
        al_load_sample("assets/sons/Zumbi/dano.wav");

    s.dano_miss =
        al_load_sample("assets/sons/Zumbi/dano_miss.wav");

    s.morte_zumbi =
        al_load_sample("assets/sons/Zumbi/morte_zumbi.wav");

    s.dano_zumbi =
        al_load_sample("assets/sons/Zumbi/dano_zumbi.wav");

    s.zumbi_knockback =
        al_load_sample("assets/sons/Zumbi/zumbi_KnockBack.ogg");


    s.som_velocista =
        al_load_sample("assets/sons/Zumbi/som_velocista.wav");

    s.ataque_velocista =
        al_load_sample("assets/sons/Zumbi/ataque_velocista.wav");

    s.dano_velocista =
        al_load_sample("assets/sons/Zumbi/dano_velocista.wav");

    s.morte_velocista =
        al_load_sample("assets/sons/Zumbi/morte_velocista.wav");

    s.dano_sofrido_velocista =
        al_load_sample("assets/sons/Zumbi/dano_sofrido_velocista.wav");


    /* Game */
    s.padrao =
        al_load_sample("assets/sons/game/padrao.wav");

    s.jogar =
        al_load_sample("assets/sons/game/jogar.wav");

    s.esc_som =
        al_load_sample("assets/sons/game/esc.wav");

    s.game_over_som =
        al_load_sample("assets/sons/game/game_over.wav");

    s.pocao_som =
        al_load_sample("assets/sons/game/pocao.wav");

    s.sair_som =
        al_load_sample("assets/sons/game/sair.wav");

    s.finish_som =
        al_load_sample("assets/sons/game/finish.wav");

    s.selecao_som =
        al_load_sample("assets/sons/game/selecao.wav");

    s.horda_som =
        al_load_sample("assets/sons/Zumbi/horda.wav");


    /* Ácido */
    s.acido_impacto =
        al_load_sample("assets/sons/Zumbi/acido/acido_impacto.wav");

    s.cuspindo_acido =
        al_load_sample("assets/sons/Zumbi/acido/cuspindo_acido.wav");

    s.dano_zumbi_acido =
        al_load_sample("assets/sons/Zumbi/acido/dano_zumbi_acido.wav");

    s.explosao_acida =
        al_load_sample("assets/sons/Zumbi/acido/explosao.wav");

    s.explosao_acida2 =
        al_load_sample("assets/sons/Zumbi/acido/explosao2.wav");

    s.morte_acido =
        al_load_sample("assets/sons/Zumbi/acido/morte_acido.wav");

    s.impacto_espada =
        al_load_sample("assets/sons/Zumbi/acido/impacto_espada.wav");


    s.padrao_tocando = 0;


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

    if (s->sair_som)
        al_destroy_sample(s->sair_som);

    if (s->finish_som)
        al_destroy_sample(s->finish_som);

    if (s->selecao_som)
        al_destroy_sample(s->selecao_som);

    if (s->horda_som)
        al_destroy_sample(s->horda_som);


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
}