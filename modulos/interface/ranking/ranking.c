#include "ranking.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

/* ================================================================== */
/*  RANKING                                                           */
/* ================================================================== */
void ordenar_ranking(float r[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (r[j] > r[j + 1])
            {
                float t = r[j];
                r[j] = r[j + 1];
                r[j + 1] = t;
            }
}

int busca_binaria(float r[], int n, float v)
{
    int lo = 0, hi = n - 1, mid;

    while (lo <= hi)
    {
        mid = (lo + hi) / 2;

        if (v < r[mid])
            hi = mid - 1;
        else
            lo = mid + 1;
    }

    return lo;
}

void carregar_ranking(Temporizador *t)
{
    FILE *f = fopen("ranking.txt", "r");
    if (!f)
        return;

    while (fscanf(f, "%f", &t->ranking[t->quantidade_scores]) == 1)
    {
        t->quantidade_scores++;
        if (t->quantidade_scores >= 10)
            break;
    }

    fclose(f);
    ordenar_ranking(t->ranking, t->quantidade_scores);
}

void salvar_ranking(Temporizador *t)
{
    FILE *f = fopen("ranking.txt", "w");
    if (!f)
        return;

    for (int i = 0; i < t->quantidade_scores; i++)
        fprintf(f, "%.2f\n", t->ranking[i]);

    fclose(f);
}

void tela_ranking(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                  ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                  Temporizador *tempo, double tempo_final)
{
    int pos = busca_binaria(tempo->ranking, tempo->quantidade_scores, (float)tempo_final);

    if (tempo->quantidade_scores < 10)
    {
        for (int i = tempo->quantidade_scores; i > pos; i--)
            tempo->ranking[i] = tempo->ranking[i - 1];

        tempo->ranking[pos] = (float)tempo_final;
        tempo->quantidade_scores++;
    }
    else if (pos < 10)
    {
        for (int i = 9; i > pos; i--)
            tempo->ranking[i] = tempo->ranking[i - 1];

        tempo->ranking[pos] = (float)tempo_final;
    }

    salvar_ranking(tempo);

    ALLEGRO_EVENT ev;

    while (1)
    {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER ||
                ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return;

        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;

        al_clear_to_color(al_map_rgb(15, 10, 5));

        float pw = 700, ph = 680;
        float px_ = LARGURA / 2.0f - pw / 2.0f;
        float py_ = ALTURA / 2.0f - ph / 2.0f;

        al_draw_filled_rounded_rectangle(px_ + 6, py_ + 6, px_ + pw + 6, py_ + ph + 6, 14, 14, al_map_rgba(0, 0, 0, 160));
        al_draw_filled_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(50, 28, 8));
        al_draw_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(218, 165, 32), 4);

        al_draw_text(fonte, al_map_rgb(255, 215, 0),
                     LARGURA / 2.0f, py_ + 20,
                     ALLEGRO_ALIGN_CENTER, "FASE CONCLUIDA!");

        char buf[80];
        sprintf(buf, "Seu tempo: %.2f s", tempo_final);
        al_draw_text(fonte_hud, al_map_rgb(255, 255, 180),
                     LARGURA / 2.0f, py_ + 80,
                     ALLEGRO_ALIGN_CENTER, buf);

        al_draw_text(fonte_hud, al_map_rgb(218, 165, 32),
                     LARGURA / 2.0f, py_ + 148,
                     ALLEGRO_ALIGN_CENTER, "TOP 10");

        for (int i = 0; i < tempo->quantidade_scores; i++)
        {
           char linha[64];
            sprintf(linha, "%d.   %.2f s", i + 1, tempo->ranking[i]);

            ALLEGRO_COLOR cor;
            if (fabsf(tempo->ranking[i] - (float)tempo_final) < 0.005f){
                cor = al_map_rgb(255, 215, 0);
            }
            else{
                cor = al_map_rgb(220, 220, 220); 
            }

            al_draw_text(fonte_hud, cor,
                         LARGURA / 2.0f, py_ + 195 + i * 44,
                         ALLEGRO_ALIGN_CENTER, linha);
        }

        al_draw_text(fonte_hud, al_map_rgb(160, 160, 160),
                     LARGURA / 2.0f, py_ + ph - 52,
                     ALLEGRO_ALIGN_CENTER,
                     "Pressione ENTER para sair");

        al_flip_display();
    }
}