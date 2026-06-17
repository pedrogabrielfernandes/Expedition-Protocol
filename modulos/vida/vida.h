#ifndef VIDAS_H
#define VIDAS_H

#include "../tipos.h"


int contar_vidas(VidaStatus *vidas);

void restaurar_vidas(VidaStatus *vidas, int quantidade);

void perder_vida(VidaStatus *vidas);


#endif