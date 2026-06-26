#ifndef MAPA_H
#define MAPA_H

#define TOTAL_MAPAS 4

/* ------------------------------------------------------------------ */
/*  Ponto de spawn individual                                           */
/* ------------------------------------------------------------------ */
typedef struct
{
    float x_min;   /* coordenada X mínima da área de spawn             */
    float x_max;   /* coordenada X máxima da área de spawn             */
    float y;       /* coordenada Y fixa do spawn                       */
} SpawnRegiao;

/* ------------------------------------------------------------------ */
/*  Configuração completa de um mapa                                   */
/* ------------------------------------------------------------------ */
typedef struct
{
    int         id;             /* identificador do mapa (0, 1, 2, 3)  */
    const char *nome;           /* nome legível, usado em debug/UI     */
    const char *arquivo_colisao;/* caminho do bitmap de colisão        */
    const char *arquivo_visual; /* caminho do bitmap visual (fundo)    */

    /* Spawn lado direito (inimigos chegam da direita)                 */
    SpawnRegiao spawn_dir;

    /* Spawn lado esquerdo / plataforma superior                       */
    SpawnRegiao spawn_esq;

    /* Posição inicial do samurai                                      */
    float jogador_spawn_x;
    float jogador_spawn_y;

    /* Limites de patrulha dos zumbis neste mapa                       */
    float patrol_x_min;
    float patrol_x_max;

    /* Limite de Y do nível "alto" (plataforma superior)               */
    float nivel_alto_y;

    /* Máximo de spawns por lado antes de forçar o outro lado          */
    int max_spawns_esq;
    int max_spawns_dir;

    /* Spawn fixo de poção (só usado quando pocao_spawn_fixo == 1).
       Quando 0, o spawn continua aleatório (comportamento padrão).    */
    int   pocao_spawn_fixo;
    float pocao_spawn_x;
    float pocao_spawn_y;
} ConfigMapa;

/* ------------------------------------------------------------------ */
/*  Acesso aos mapas                                                    */
/* ------------------------------------------------------------------ */

/* Retorna ponteiro para a configuração do mapa de índice 'id'.
   Retorna NULL se id for inválido.                                    */
const ConfigMapa *mapa_obter(int id);

/* Retorna o número total de mapas disponíveis.                        */
int mapa_total(void);

#endif /* MAPA_H */