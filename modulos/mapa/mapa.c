#include "mapa.h"
#include <stddef.h>  /* NULL */

static const ConfigMapa g_mapas[TOTAL_MAPAS] = {

    /* ---------------------------------------------------------------- */
    /*  MAPA 0 ? background2 / colisao2                                  */
    /* ---------------------------------------------------------------- */
    {
        .id              = 0,
        .arquivo_colisao = "assets/cenarios/colisao2.png",
        .arquivo_visual  = "assets/cenarios/background2.png",

        .spawn_dir = { 1700.0f, 1800.0f, 655.0f },
        .spawn_esq = { 15.0f, 195.0f, 210.0f },

        .jogador_spawn_x = 105.0f,
        .jogador_spawn_y = 210.0f,

        .patrol_x_min  = 10.0f,
        .patrol_x_max  = 1800.0f,
        .nivel_alto_y  = 380.0f,

        .max_spawns_esq = 25,
        .max_spawns_dir = 25,

        /* poção aleatória (padrão) */
        .pocao_spawn_fixo = 0,
        .pocao_spawn_x    = 0.0f,
        .pocao_spawn_y    = 0.0f,
    },

    /* ---------------------------------------------------------------- */
    /*  MAPA 1 ? background3 / colisao3                                  */
    /*  Poção nasce em posição fixa: X=1310, Y=10                       */
    /* ---------------------------------------------------------------- */
    {
        .id              = 1,
        .arquivo_colisao = "assets/cenarios/colisao3.png",
        .arquivo_visual  = "assets/cenarios/background3.png",

        .spawn_dir = { 1500.0f, 1650.0f, 650.0f },
        .spawn_esq = { 69.0f, 150.0f, 650.0f },

        .jogador_spawn_x = 69.0f,
        .jogador_spawn_y = 650.0f,

        .patrol_x_min  = 80.0f,
        .patrol_x_max  = 1700.0f,
        .nivel_alto_y  = 320.0f,

        .max_spawns_esq = 20,
        .max_spawns_dir = 30,

        /* poção em posição fixa */
        .pocao_spawn_fixo = 1,
        .pocao_spawn_x    = 1310.0f,
        .pocao_spawn_y    = 10.0f,
    },

    /* ---------------------------------------------------------------- */
    /*  MAPA 2 ? background4 / colisao4                                  */
    /* ---------------------------------------------------------------- */
    {
        .id              = 2,
        .arquivo_colisao = "assets/cenarios/colisao4.png",
        .arquivo_visual  = "assets/cenarios/background4.png",

        .spawn_dir = { 1700.0f, 1800.0f, 590.0f },
        .spawn_esq = { 20.0f, 150.0f, 590.0f },

        .jogador_spawn_x = 60.0f,
        .jogador_spawn_y = 620.0f,

        .patrol_x_min  = 20.0f,
        .patrol_x_max  = 1900.0f,
        .nivel_alto_y  = 400.0f,

        .max_spawns_esq = 25,
        .max_spawns_dir = 25,

        /* poção aleatória (padrão) */
        .pocao_spawn_fixo = 0,
        .pocao_spawn_x    = 0.0f,
        .pocao_spawn_y    = 0.0f,
    },

    /* ---------------------------------------------------------------- */
    /*  MAPA 3 ? background5 / colisao5                                  */
    /*  Fase do BOSS: spawns de zumbis zerados.                          */
    /*  O boss é instanciado diretamente no main.c.                      */
    /* ---------------------------------------------------------------- */
    {
        .id              = 3,
        .arquivo_colisao = "assets/cenarios/colisao5.png",
        .arquivo_visual  = "assets/cenarios/background5.png",

        .spawn_dir = { 1400.0f, 1550.0f, 670.0f },
        .spawn_esq = { 50.0f, 250.0f, 440.0f },

        .jogador_spawn_x = 69.0f,
        .jogador_spawn_y = 640.0f,

        .patrol_x_min  = 50.0f,
        .patrol_x_max  = 1750.0f,
        .nivel_alto_y  = 520.0f,

        /* Spawns zerados: a horda não gera zumbis nessa fase */
        .max_spawns_esq = 0,
        .max_spawns_dir = 0,

        /* poção aleatória (padrão) */
        .pocao_spawn_fixo = 0,
        .pocao_spawn_x    = 0.0f,
        .pocao_spawn_y    = 0.0f,
    },
};

/* ================================================================== */
/*  Implementação das funções públicas                                  */
/* ================================================================== */

const ConfigMapa *mapa_obter(int id)
{
    if (id < 0 || id >= TOTAL_MAPAS)
        return NULL;
    return &g_mapas[id];
}

int mapa_total(void)
{
    return TOTAL_MAPAS;
}