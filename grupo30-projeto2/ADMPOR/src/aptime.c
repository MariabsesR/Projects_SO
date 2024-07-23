#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memory.h"
#include "aptime.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que recebe uma estrutura timespec e registar o instante no
 * tempo em que foi chamada, que corresponde à criação de uma operação na main,
 * à chegada de uma operação ao cliente, à chegada de uma operação ao intermediário e à chegada
 * de uma operação à empresa
 */
void get_time(struct timespec *tp)
{

    if (clock_gettime(CLOCK_REALTIME, tp) == -1)
    {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }
}
