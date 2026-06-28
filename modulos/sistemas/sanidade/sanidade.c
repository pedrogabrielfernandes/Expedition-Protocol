#include "sanidade.h"
#include <allegro5/allegro_primitives.h>

#include <math.h>

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

        float dur_recup =
            (float)(TEMPO_RECUPERACAO_TOTAL - TEMPO_INICIO_RECUPERACAO);


        float t_recup =
            (float)(sem_matar - TEMPO_INICIO_RECUPERACAO);


        float progresso = t_recup / dur_recup;


        if (progresso > 1.0f)
            progresso = 1.0f;


        float novo_valor =
            alvo + (MAX_SANIDADE - alvo) * progresso;


        if (novo_valor > san->valor)
            san->valor = novo_valor;


        if (san->valor > MAX_SANIDADE)
            san->valor = MAX_SANIDADE;


        if (progresso < 1.0f)
        {
            san->regenerando = 1;
        }
            else
            {
            san->regenerando = 0;
            }


        if (san->valor >= MAX_SANIDADE)
        {
            san->valor = MAX_SANIDADE;

            san->ciclo_base =
                san->zumbis_mortos;

            san->regenerando = 0;
        }
    }

    else
    {
        san->regenerando = 0;
    }
}



void desenhar_overlay_sanidade(Sanidade *san)
{
    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS)
        return;


    double agora = al_get_time();

    double sem_matar =
        agora - san->ultimo_abate;


    float frac_overlay;


    if (sem_matar < TEMPO_INICIO_RECUPERACAO)

        frac_overlay = 1.0f;


    else if (sem_matar < TEMPO_RECUPERACAO_TOTAL)
    {
        float t_fade =
            (float)(sem_matar - TEMPO_INICIO_RECUPERACAO);


        float dur_fade =
            (float)(TEMPO_RECUPERACAO_TOTAL -
                    TEMPO_INICIO_RECUPERACAO);


        frac_overlay =
            1.0f - (t_fade / dur_fade);


        if (frac_overlay < 0.0f)
            frac_overlay = 0.0f;
    }


    else
        return;



    float pulso =
        0.5f +
        0.5f * sinf((float)agora * 5.0f);



    unsigned char alpha =
        (unsigned char)
        ((60.0f +
        120.0f * frac_overlay)
        * pulso *
        frac_overlay);



    if (alpha == 0)
        return;



    al_draw_filled_rectangle(
        0,
        0,
        LARGURA,
        ALTURA,
        al_map_rgba(200,0,0,alpha)
    );
}



void desenhar_aviso_sanidade(
    Sanidade *san,
    ALLEGRO_FONT *fonte,
    double t
)
{
    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS ||
        san->game_over)

        return;


    double sem_matar =
        t - san->ultimo_abate;


    if (sem_matar >= TEMPO_RECUPERACAO_TOTAL)
        return;


    if ((int)(t / 0.6) % 2 == 0)
        return;



    float tx = LARGURA - 616.0f;

    float ty = ALTURA - 105.0f;



    al_draw_text(
        fonte,
        al_map_rgba(0,0,0,200),
        tx + 2,
        ty + 2,
        ALLEGRO_ALIGN_CENTER,
        "INSANIDADE!"
    );


    al_draw_text(
        fonte,
        al_map_rgba(0,0,0,200),
        tx + 2,
        ty + 38,
        ALLEGRO_ALIGN_CENTER,
        "Pare de matar!"
    );


    al_draw_text(
        fonte,
        al_map_rgb(255,60,60),
        tx,
        ty,
        ALLEGRO_ALIGN_CENTER,
        "INSANIDADE!"
    );


    al_draw_text(
        fonte,
        al_map_rgb(255,180,180),
        tx,
        ty + 38,
        ALLEGRO_ALIGN_CENTER,
        "Pare de matar!"
    );
}



void desenhar_mensagem_central_insanidade(
    Sanidade *san,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_hud,
    double t
)
{

    if (zumbis_no_ciclo(san) < AVISO_ZUMBIS ||
        san->game_over)

        return;



    double sem_matar =
        t - san->ultimo_abate;



    if (sem_matar >= TEMPO_RECUPERACAO_TOTAL)
        return;



    float frac_overlay;


    if (sem_matar < TEMPO_INICIO_RECUPERACAO)

        frac_overlay = 1.0f;


    else
    {
        float t_fade =
            (float)(sem_matar -
            TEMPO_INICIO_RECUPERACAO);


        float dur_fade =
            (float)(TEMPO_RECUPERACAO_TOTAL -
            TEMPO_INICIO_RECUPERACAO);



        frac_overlay =
            1.0f -
            (t_fade / dur_fade);



        if (frac_overlay < 0.0f)

            frac_overlay = 0.0f;
    }



    if (frac_overlay < 0.05f)

        return;



    float pulso =
        0.7f +
        0.3f * sinf((float)t * 3.0f);



    unsigned char alpha_txt =
        (unsigned char)
        (255.0f *
        frac_overlay *
        pulso);



    float cx = LARGURA / 2.0f;

    float cy = ALTURA / 2.0f - 40.0f;


    float box_w = 840.0f;

    float box_h = 110.0f;



    al_draw_filled_rounded_rectangle(
        cx - box_w / 2.0f,
        cy - 20.0f,
        cx + box_w / 2.0f,
        cy + box_h,
        12,
        12,
        al_map_rgba(0,0,0,
        (unsigned char)(160.0f *
        frac_overlay))
    );



    al_draw_text(
        fonte,
        al_map_rgba(255,40,40,alpha_txt),
        cx,
        cy,
        ALLEGRO_ALIGN_CENTER,
        "VOCE ESTA INSANO!"
    );


    al_draw_text(
        fonte_hud,
        al_map_rgba(255,200,200,alpha_txt),
        cx,
        cy + 52.0f,
        ALLEGRO_ALIGN_CENTER,
        "Pare de eliminar inimigos ou perca sua mente!"
    );
}