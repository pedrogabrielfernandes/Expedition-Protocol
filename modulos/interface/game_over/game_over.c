#include "game_over.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "../../sistemas/sons/sons.h"

int tela_game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_TIMER *timer,
                   ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_hud,
                   const char *motivo, Sons *sons)
{
    al_flush_event_queue(queue);

    ALLEGRO_EVENT ev;
    int opcao     = 0;
    int opcao_ant = 0;

    float mouse_x = 0.0f, mouse_y = 0.0f;

    /* Dimensões do painel */
    float pw = 910, ph = 340;
    float px_ = LARGURA / 2.0f - pw / 2.0f;
    float py_ = ALTURA  / 2.0f - ph / 2.0f;

    /* Hitboxes dos botões */
    float btn_w      = 180.0f, btn_h = 40.0f;
    float btn_rei_cx = LARGURA / 2.0f - 160.0f;
    float btn_sai_cx = LARGURA / 2.0f + 160.0f;
    float btn_y      = py_ + 186.0f;

    float rei_x = btn_rei_cx - btn_w / 2.0f;
    float sai_x = btn_sai_cx - btn_w / 2.0f;

    while (1)
    {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            return 0;

        /* ---- mouse: movimento / hover ---- */
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            mouse_x = (float)ev.mouse.x;
            mouse_y = (float)ev.mouse.y;

            if (mouse_x >= rei_x && mouse_x <= rei_x + btn_w &&
                mouse_y >= btn_y  && mouse_y <= btn_y  + btn_h)
                opcao = 0;
            else if (mouse_x >= sai_x && mouse_x <= sai_x + btn_w &&
                     mouse_y >= btn_y  && mouse_y <= btn_y  + btn_h)
                opcao = 1;
        }

        /* ---- mouse: clique ---- */
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)
        {
            float mx = (float)ev.mouse.x, my = (float)ev.mouse.y;

            if (mx >= rei_x && mx <= rei_x + btn_w &&
                my >= btn_y  && my <= btn_y  + btn_h)
            {
                tocar(sons->jogar);
                return 1;
            }
            if (mx >= sai_x && mx <= sai_x + btn_w &&
                my >= btn_y  && my <= btn_y  + btn_h)
            {
                tocar(sons->sair_som);
                return 0;
            }
        }

        /* ---- teclado ---- */
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
                if (opcao == 0) { tocar(sons->jogar);    return 1; }
                else            { tocar(sons->sair_som); return 0; }
            }

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                tocar(sons->sair_som);
                return 0;
            }
        }

        /* Som de hover ao trocar de opção */
        if (opcao != opcao_ant)
        {
            tocar(sons->selecao_som);
            opcao_ant = opcao;
        }

        if (ev.type != ALLEGRO_EVENT_TIMER)
            continue;
        if (!al_is_event_queue_empty(queue))
            continue;

        /* ---- desenho ---- */
        al_clear_to_color(al_map_rgb(10, 5, 5));

        al_draw_filled_rounded_rectangle(px_ + 6, py_ + 6, px_ + pw + 6, py_ + ph + 6,
                                         14, 14, al_map_rgba(0, 0, 0, 180));
        al_draw_filled_rounded_rectangle(px_, py_, px_ + pw, py_ + ph,
                                         14, 14, al_map_rgb(60, 10, 10));
        al_draw_rounded_rectangle(px_, py_, px_ + pw, py_ + ph,
                                  14, 14, al_map_rgb(200, 30, 30), 4);

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

        /* Botão REINICIAR */
        {
            int hover = (opcao == 0);
            al_draw_text(fonte, hover ? al_map_rgb(255, 215, 0) : al_map_rgb(180, 180, 180),
                         btn_rei_cx, btn_y,
                         ALLEGRO_ALIGN_CENTER,
                         hover ? "> REINICIAR <" : "REINICIAR");
        }

        /* Botão SAIR */
        {
            int hover = (opcao == 1);
            al_draw_text(fonte, hover ? al_map_rgb(255, 215, 0) : al_map_rgb(180, 180, 180),
                         btn_sai_cx, btn_y,
                         ALLEGRO_ALIGN_CENTER,
                         hover ? "> SAIR <" : "SAIR");
        }

        al_draw_text(fonte_hud, al_map_rgb(130, 130, 130),
                     LARGURA / 2.0f, py_ + ph - 46,
                     ALLEGRO_ALIGN_CENTER,
                     "A/D ou Setas para navegar  |  ENTER para confirmar");

        al_flip_display();
    }
}