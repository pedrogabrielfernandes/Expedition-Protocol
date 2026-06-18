#ifndef SONS_H
#define SONS_H

#include "../../tipos.h"


void tocar(ALLEGRO_SAMPLE *s);


void tocar_pitch(ALLEGRO_SAMPLE *s, float pitch);


void tocar_musica_fundo(Sons *s);


void parar_musica_fundo(Sons *s);


void aplicar_volume_musica(Sons *s);


Sons carregar_sons(void);


void destruir_sons(Sons *s);

extern Sons *g_sons_ativo;

#endif 