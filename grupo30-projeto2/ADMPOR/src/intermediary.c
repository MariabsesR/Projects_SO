#include "stdio.h"
#include "intermediary.h"
#include "main.h"
#include "memory.h"
#include "aptime.h"
#include "apsignal.h"
#include <signal.h>

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função principal de um Intermediário. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação dos clientes e se a mesma tiver id
 * diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
 * envia a mesma para as empresas. Operações com id igual a -1 são
 * ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi
 * dada ordem de terminação do programa, portanto deve-se fazer return do
 * número de operações processadas. Para efetuar estes passos, pode usar os
 * outros métodos auxiliares definidos em intermediary.h.
 */
int execute_intermediary(int interm_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{

    int counter = 0;

    // Ignora o sinal de interrupção (SIGINT) para que os processos dos intermediários não sejam encerrados abruptamente
    signal(SIGINT, SIG_IGN);

    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while (*(data)->terminate != 1)
    {
        intermediary_receive_operation(op, buffers, data, sems);

        if (*data->terminate == 0 && interm_id != -1 && op->id != -1 && op->receiving_interm == -1)
        {
            printf("Intermediário recebeu pedido!\n");
            intermediary_process_operation(op, interm_id, data, &counter, sems);
            // vê se a empresa requisitada existe, se sim envia-a para o buffer
            if (op->requested_enterp >= 0 && op->requested_enterp <= data->n_enterprises - 1)
            {
                intermediary_send_answer(op, buffers, data, sems);
            }
        }
    }
    destroy_dynamic_memory(op);
    return counter;
}

/* Função que lê uma operação do buffer de memória partilhada entre clientes e intermediários.
 * Antes de tentar ler a operação, deve verificar se data->terminate tem valor 1.
 * Em caso afirmativo, retorna imediatamente da função.
 */
void intermediary_receive_operation(struct operation *op, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        // faz wait dos semáforos que controlam o acesso para leitura do buffer entre a o cliente e o intermediário
        consume_begin(sems->client_interm);
        read_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
        // faz post dos semáforos que controlam o acesso para leitura do buffer entre a o cliente e o intermediário
        consume_end(sems->client_interm);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_intermediary para o id
 * passado como argumento, alterando o estado da mesma para 'I' (intermediary), e
 * incrementando o contador de operações. Atualiza também a operação na estrutura data.
 */
void intermediary_process_operation(struct operation *op, int interm_id, struct main_data *data, int *counter, struct semaphores *sems)
{
    // faz wait do semáforo de acesso a data
    semaphore_mutex_lock(sems->results_mutex);

    // escreve o tempo atual em que o intermediário processa a operação
    get_time(&op->intermed_time);

    op->receiving_interm = interm_id;

    op->status = 'I';

    (*counter)++;

    for (int i = 0; i < MAX_RESULTS; i++)
    {
        if (data->results[i].id == op->id)
        {
            data->results[i] = *op;
            break;
        }
    }

    // faz post do semáforo de acesso a data
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que escreve uma operação no buffer de memória partilhada entre
 * intermediários e empresas.
 */
void intermediary_send_answer(struct operation *op, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    // faz wait dos semáforos que controlam o acesso para escrita do buffer entre o intermediário e a empresa
    produce_begin(sems->interm_enterp);
    write_interm_enterp_buffer(buffers->interm_enterp, data->buffers_size, op);
    // faz post dos semáforos que controlam o acesso para escrita do buffer entre o intermediário e a empresa
    produce_end(sems->interm_enterp);
}