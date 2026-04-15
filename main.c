#include <allegro5/allegro.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/events.h>
#include <allegro5/system.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#define largura 1920
#define altura 1080
#define FPS 30
//sprite personagem: x = 96 y = 84

int main() {
    if (!al_init()){
        printf("falha na inicialização do jogo.");
        return 1;
    }
    if (!al_init_font_addon()){
        printf("falha na inicialização das fontes automaticas");
        return 1;
    }
    if (!al_init_ttf_addon()) {
        printf("falha na inicialização das fontes");
        return 1;
    }
    if (!al_init_image_addon()) {
        printf("falha no carramento da imagem");
        return 1;
    }
    if (!al_install_keyboard()){
        printf("falha na inicializaÃ§Ã£o do teclado.");
    }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_FONT *fonte = al_load_ttf_font("assets/arial.ttf", 24, 0);
        if (!fonte) {
            printf("falha na inicialização da fonte arial");
            return 1;
        }

    //definindo o tamanho da janela e onde ela vai aparecer
    al_set_new_window_position(320, 180);
    ALLEGRO_DISPLAY *display = al_create_display(1920, 1080);

    al_register_event_source(queue, al_get_display_event_source(display));

    ALLEGRO_TIMER *timer = al_create_timer(1.0/FPS);
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    ALLEGRO_BITMAP *bg = al_load_bitmap("assets/background.png");
    if (!bg) {
        printf("falha na inicialização do background");
        return 1;
    }

    //um evento
    ALLEGRO_EVENT evento;

    //criando um inteiro para o laço de repetição
    int rodando = 1;

    //o laco de repeticao que vai fazer o jogo rodar ate fechar
    while (rodando){
            al_wait_for_event(queue, &evento);
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                rodando = 0;
            }
        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(fonte, al_map_rgb(0, 0, 0), 100, 100, 0, "Horario: 15:30");
        al_flip_display();
    }

    ALLEGRO_EVENT evento;
    int rodando = 1;
    float frame = 0.f;
    int pos_x = 462;
    int pos_y = 707;
    int current_frame_y = 0;
    int direcao = 0;
    float vel_y = 0;
    int no_chao = 1;
    int movendo = 0;
    float gravidade = 1.0;
    float forca_pulo =  -15;
    ALLEGRO_KEYBOARD_STATE state;

    al_set_window_title(display, "Expedition");

    while (rodando){
        al_wait_for_event(queue, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;
        }

        if (evento.type == ALLEGRO_EVENT_TIMER){
            frame += 0.3f;

            movendo = 0;

            al_clear_to_color(al_map_rgb(255, 255, 255));
            al_draw_bitmap(bg, 0, 0, 0);
            al_get_keyboard_state(&state);

            if (al_key_down(&state, ALLEGRO_KEY_D)){
                pos_x += 5;
                movendo = 1;
                direcao = 0;
                
            }
            if (al_key_down(&state, ALLEGRO_KEY_A)){
                pos_x -= 5;
                movendo = 1;
                direcao = ALLEGRO_FLIP_HORIZONTAL;
            }
            if (al_key_down(&state, ALLEGRO_KEY_W) && no_chao){
                vel_y = forca_pulo;
                no_chao = 0;
            }
                vel_y += gravidade;
                pos_y += vel_y;

            
                if (pos_y >= 707){
                    pos_y = 707;
                    vel_y = 0;
                    no_chao = 1;
                }


            if (!no_chao){
                if (frame >= 5) frame = 0;
                al_draw_bitmap_region(jump, 96 * (int)frame, current_frame_y, 96, 84, pos_x, pos_y, direcao);
            }
            else if (movendo){
                if (frame >= 8) frame = 0;
                al_draw_bitmap_region(run, 96 * (int)frame, current_frame_y, 96, 84, pos_x, pos_y, direcao);
            }
            else{
                if (frame >= 7) frame = 0;
                al_draw_bitmap_region(idle, 96 * (int)frame, current_frame_y, 96, 84, pos_x, pos_y, direcao);
            }

            al_draw_text(fonte, al_map_rgb(0, 0, 0), 1750, 104, 0, "HORARIO");
            al_flip_display();
        }
    }

    al_destroy_bitmap(idle);
    al_destroy_bitmap(run);
    al_destroy_bitmap(jump);
    al_destroy_bitmap(bg);
    al_destroy_display(display);
    al_destroy_font(fonte);
    al_destroy_event_queue(queue);

    return 0;
}

//se tem duvidas, leia as instuÃ§Ãµes na pasta do jogo.

//(Linux) -> sudo "..." -S github-cli
//(Linux) -> gh auth login - faz o login
//(Linux) -> git clone https://github.com/pedrogabrielfernandes/jogo
//após cada alteração que fizerem dê ctrl+s atualizem o repositório do github
//exemplo: 
//git add .
//git commit -m "o que você alterou"
//git push
//logo após atualize seu repositorio e seu gitclone
