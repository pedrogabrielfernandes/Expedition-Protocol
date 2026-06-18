#include "game_over.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

int tela_game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                   ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                   const char *motivo)
{
    al_flush_event_queue(queue);

    ALLEGRO_EVENT ev;
    int opcao = 0;

    while (1)
    {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return 0;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT ||
                ev.keyboard.keycode == ALLEGRO_KEY_A)
                opcao = 0;

            if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT ||
                ev.keyboard.keycode == ALLEGRO_KEY_D)
                opcao = 1;

            if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER ||
                ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                if (opcao == 0)
                    return 1;
                else
                    return 0;
            }

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return 0;
        }

        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;

        al_clear_to_color(al_map_rgb(10, 5, 5));

        float pw = 910, ph = 340;
        float px_ = LARGURA / 2.0f - pw / 2.0f;
        float py_ = ALTURA / 2.0f - ph / 2.0f;


        al_draw_filled_rounded_rectangle(px_ + 6, py_ + 6, px_ + pw + 6, py_ + ph + 6, 14, 14, al_map_rgba(0, 0, 0, 180));
        al_draw_filled_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(60, 10, 10));
        al_draw_rounded_rectangle(px_, py_, px_ + pw, py_ + ph, 14, 14, al_map_rgb(200, 30, 30), 4);

        al_draw_text(fonte, al_map_rgb(255, 60, 60),
                     LARGURA / 2.0f, py_ + 28,
                     ALLEGRO_ALIGN_CENTER, "GAME OVER");

        al_draw_line(px_ + 40, py_ + 88, px_ + pw - 40, py_ + 88,
                     al_map_rgb(200, 30, 30), 2);

        al_draw_text(fonte_hud, al_map_rgb(255, 200, 200),
                     LARGURA / 2.0f, py_ + 102,
                     ALLEGRO_ALIGN_CENTER, motivo);

        al_draw_line(px_ + 40, py_ + 158, px_ + pw - 40, py_ + 158,
                     al_map_rgb(120, 20, 20), 1);

        
        const char *tr;
        ALLEGRO_COLOR cor_tr;
        if (opcao == 0) {
            tr = "> REINICIAR <";
            cor_tr = al_map_rgb(255, 215, 0); 
        } else {
            tr = "REINICIAR";
            cor_tr = al_map_rgb(180, 180, 180);
        }

        al_draw_text(fonte, cor_tr,
                     LARGURA / 2.0f - 160, py_ + 186,
                     ALLEGRO_ALIGN_CENTER, tr);

       
        const char *ts;
        ALLEGRO_COLOR cor_ts;
        if (opcao == 1) {
            ts = "> SAIR <";
            cor_ts = al_map_rgb(255, 215, 0); 
        } else {
            ts = "SAIR";
            cor_ts = al_map_rgb(180, 180, 180); 
        }

        al_draw_text(fonte, cor_ts,
                     LARGURA / 2.0f + 160, py_ + 186,
                     ALLEGRO_ALIGN_CENTER, ts);

        al_draw_text(fonte_hud, al_map_rgb(130, 130, 130),
                     LARGURA / 2.0f, py_ + ph - 46,
                     ALLEGRO_ALIGN_CENTER,
                     "A/D ou Setas para navegar  |  ENTER para confirmar");

        al_flip_display();
    }
}