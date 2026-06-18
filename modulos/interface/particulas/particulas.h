#ifndef PARTICULAS_H
#define PARTICULAS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>

#include "../../tipos.h"

Particula *criar_matriz_decorativa(int linhas, int colunas);
void desenhar_matriz_fundo(Particula *mat, int total, double t);
void liberar_matriz(Particula *mat, int linhas);

#endif