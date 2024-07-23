#include <stdio.h>
#include <time.h>
#include <signal.h>
#include "client.h"
#include "main.h"
#include "memory.h"
#include "aptime.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função principal de um Cliente. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação da main e se data->terminate ainda
 * for igual a 0, processa-a. Operações com id igual a -1 são ignoradas
 * (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
 * de terminação do programa, portanto deve-se fazer return do número de
 * operações processadas. Para efetuar estes passos, pode usar os outros
 * métodos auxiliares definidos em client.h.
 */
int execute_client(int client_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{

    int counter = 0;

    // Ignora o sinal de interrupção (SIGINT) para que os processos dos clientes não sejam encerrados abruptamente
    signal(SIGINT, SIG_IGN);

    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while (*data->terminate != 1)
    {

        client_get_operation(op, client_id, buffers, data, sems);

        if (*data->terminate == 0)
        {
            // se for o cliente correto, entra no if e prossegue
            if (op->id != -1)
            {
                printf("Cliente recebeu pedido!\n");
                client_process_operation(op, client_id, data, &counter, sems);
                client_send_operation(op, buffers, data, sems);
            }
            else
            {
                // caso contrário(ou seja, não ser o cliente correto) volta a fazer o mesmo que a main, ou seja, volta a sinalizar que outro cliente pode executar
                // faz post dos semáforos que controlam o acesso para escrita do buffer entre a main e o cliente
                produce_end(sems->main_client);
            }
        }
    }

    destroy_dynamic_memory(op);
    return counter;
}

/* Função que lê uma operação do buffer de memória partilhada entre a main
 * e clientes que seja direcionada a client_id. Antes de tentar ler a operação,
 * deve verificar se data->terminate tem valor 1. Em caso afirmativo, retorna
 * imediatamente da função.
 */
void client_get_operation(struct operation *op, int client_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        // faz wait dos semáforos que controlam o acesso para leitura do buffer entre a main e o cliente
        consume_begin(sems->main_client);
        read_main_client_buffer(buffers->main_client, client_id, data->buffers_size, op);
        // faz post dos semáforos que controlam o acesso para leitura do buffer entre a main e o cliente
        consume_end(sems->main_client);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_client para
 * o id passado como argumento, alterando o estado da mesma para 'C' (client), e
 * incrementando o contador de operações. Atualiza também a operação na
 * estrutura data.
 */
void client_process_operation(struct operation *op, int client_id, struct main_data *data, int *counter, struct semaphores *sems)
{

    // faz wait do semáforo de acesso a data
    semaphore_mutex_lock(sems->results_mutex);

    // escreve o tempo atual em que o cliente processa a operação
    get_time(&op->client_time);

    op->receiving_client = client_id;
    op->status = 'C';
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
 * clientes e intermediários.
 */
void client_send_operation(struct operation *op, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    // faz wait dos semáforos que controlam o acesso para escrita do buffer entre o cliente e o intermediário
    produce_begin(sems->client_interm);
    write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
    // faz post dos semáforos que controlam o acesso para escrita do buffer entre o cliente e o intermediário
    produce_end(sems->client_interm);
}