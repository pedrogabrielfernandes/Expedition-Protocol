#include "vida.h"

#include <string.h>


int contar_vidas(VidaStatus *vidas)
{
    int c = 0;

    for (int i = 0; i < MAX_VIDAS; i++)
    {
        if (vidas[i].ativa)
            c++;
    }

    return c;
}



void restaurar_vidas(VidaStatus *vidas, int quantidade)
{
    int restauradas = 0;


    for (int i = 0;
         i < MAX_VIDAS && restauradas < quantidade;
         i++)
    {
        if (!vidas[i].ativa)
        {
            vidas[i].ativa = 1;

            strcpy(vidas[i].status, "Inteira");

            restauradas++;
        }
    }
}



void perder_vida(VidaStatus *vidas)
{
    for (int i = MAX_VIDAS - 1; i >= 0; i--)
    {
        if (vidas[i].ativa)
        {
            vidas[i].ativa = 0;

            strcpy(vidas[i].status, "Perdida");

            break;
        }
    }
}