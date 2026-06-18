#include "particulas.h"

/* ================================================================== */
/*  PARTÍCULAS DECORATIVAS                                            */
/* ================================================================== */
Particula *criar_matriz_decorativa(int linhas, int colunas)
{
    int total = linhas * colunas;

    Particula *mat = (Particula *)malloc(total * sizeof(Particula));

    for (int i = 0; i < total; i++)
    {
        mat[i].x = (float)(rand() % LARGURA);
        mat[i].y = (float)(rand() % ALTURA);
        mat[i].y_origem = mat[i].y;
        mat[i].vy = 0.8f + (float)(rand() % 120) / 100.0f;
        mat[i].fase = (float)(rand() % 628) / 100.0f;
        mat[i].escala = 0.8f + (float)(rand() % 15) / 10.0f;
    }

    return mat;
}

void desenhar_matriz_fundo(Particula *mat, int total, double t)
{
    for (int i = 0; i < total; i++)
    {
        mat[i].y += mat[i].vy;
        mat[i].x += mat[i].vy * 0.5f;

        if (mat[i].y > (float)ALTURA + 4.0f || mat[i].x > (float)LARGURA + 4.0f)
        {
            mat[i].y = -4.0f;
            mat[i].x = (float)(rand() % LARGURA);
        }

        float pulso = 0.6f + 0.4f * sinf((float)t * 1.2f + mat[i].fase);
        unsigned char alpha = (unsigned char)(30.0f + 60.0f * pulso);

        float r = mat[i].escala;
        float x2 = mat[i].x + mat[i].vy * 0.8f;
        float y2 = mat[i].y + mat[i].vy * 1.6f;

        al_draw_line(mat[i].x, mat[i].y,
                     x2, y2,
                     al_map_rgba(200, 180, 120, alpha),
                     r);
    }
}

void liberar_matriz(Particula *mat, int linhas)
{
    (void)linhas;
    free(mat);
}