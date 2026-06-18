#ifndef SONS_H
#define SONS_H

#include "../../tipos.h"

/* Toca um efeito sonoro respeitando volume_sfx e mudo_sfx */
void tocar(ALLEGRO_SAMPLE *s);

/* Toca com pitch personalizado respeitando volume_sfx e mudo_sfx */
void tocar_pitch(ALLEGRO_SAMPLE *s, float pitch);

/* Inicia a música de fundo em loop (respeita volume_musica e mudo_musica) */
void tocar_musica_fundo(Sons *s);

/* Para a música de fundo */
void parar_musica_fundo(Sons *s);

/* Aplica o volume_musica atual à instância em reprodução (chame após alterar o slider) */
void aplicar_volume_musica(Sons *s);

/* Carrega todos os samples e inicializa volume_sfx/volume_musica com VOLUME_PADRAO */
Sons carregar_sons(void);

/* Libera todos os recursos de sons */
void destruir_sons(Sons *s);

/*
 * Ponteiro global para o Sons ativo, necessário para que tocar() e
 * tocar_pitch() consultem volume_sfx / mudo_sfx sem receber Sons* em
 * cada chamada (a assinatura pública não muda).
 */
extern Sons *g_sons_ativo;

#endif /* SONS_H */