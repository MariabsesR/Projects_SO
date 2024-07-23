#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "apsignal.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

extern struct main_data *data;
extern struct comm_buffers *buffers;
extern struct semaphores *sems;
extern int *count;

/*
 * Função responsável por manipular sinais.
 * Define os handlers para os sinais SIGINT e SIGALRM,
 * e configura um alarme para o tempo especificado.
 */
void signal_handler()
{
    signal(SIGINT, stopBySignal);
    signal(SIGALRM, alarm_handler);

    // arma o alarme com um dado tempo em segundos
    alarm(data->alarm_time);
}

/* Função invocada com o sinal SIGINT, proveniente do Crtl+C e que efetua um
 * fecho normal do programa através da chamada à função stop_execution
 */
void stopBySignal()
{
    stop_execution(data, buffers, sems);
}

/* Função invocada com o sinal SIGALRM que imprime para o ecrã o estado atual de todos
 * os pedidos, finalizados, em andamento ou agendados
 */
void alarm_handler()
{
    int index = *count;

    // percorre todas as operações
    for (int i = 0; i < index; i++)
    {
        char cStatus = data->results[i].status;

        if (cStatus == 'M')
        {
            printf("op:%d status:%c start_time: %ld\n", i, data->results[i].status, data->results[i].start_time.tv_sec);
        }
        else if (cStatus == 'C')
        {
            printf("op:%d status:%c start_time: %ld client:%d client_time: %ld\n", i, data->results[i].status, data->results[i].start_time.tv_sec, data->results[i].receiving_client, data->results[i].client_time.tv_sec);
        }
        else if (cStatus == 'I')
        {
            printf("op:%d status:%c start_time: %ld client:%d client_time: %ld intermediary:%d intermediary_time: %ld\n", i, data->results[i].status, data->results[i].start_time.tv_sec, data->results[i].receiving_client, data->results[i].client_time.tv_sec, data->results[i].receiving_interm, data->results[i].intermed_time.tv_sec);
        }
        else if (cStatus == 'E' || cStatus == 'A')
        {
            printf("op:%d status:%c start_time: %ld client:%d client_time: %ld intermediary:%d intermediary_time: %ld enterprise:%d enterprise_time: %ld\n", i, data->results[i].status, data->results[i].start_time.tv_sec, data->results[i].receiving_client, data->results[i].client_time.tv_sec, data->results[i].receiving_interm, data->results[i].intermed_time.tv_sec, data->results[i].receiving_enterp, data->results[i].enterp_time.tv_sec);
        }
    }

    // arma o alarme com um dado tempo em segundos
    alarm(data->alarm_time);
}
