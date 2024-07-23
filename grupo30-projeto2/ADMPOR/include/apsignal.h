#ifndef APSIGNAL_H_GUARD
#define APSIGNAL_H_GUARD

#include "main.h"
#include "memory.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/*
 * Função responsável por manipular sinais.
 * Define os handlers para os sinais SIGINT e SIGALRM,
 * e configura um alarme para o tempo especificado.
 */
void signal_handler();

/* Função invocada com o sinal SIGINT, proveniente do Crtl+C e que efetua um
 * fecho normal do programa através da chamada à função stop_execution
 */
void stopBySignal();

/* Função invocada com o sinal SIGALRM que imprime para o ecrã o estado atual de todos
 * os pedidos, finalizados, em andamento ou agendados
 */
void alarm_handler();

#endif