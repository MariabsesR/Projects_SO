#include "main.h"
#include "memory.h"
#include "stdio.h"
#include "aptime.h"
#include "apsignal.h"
#include <signal.h>

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que lê uma operação do buffer de memória partilhada entre
 * os intermediários e as empresas que seja direcionada à empresa enterprise_id.
 * Antes de tentar ler a operação, o processo deve verificar se data->terminate
 * tem valor 1. Em caso afirmativo, retorna imediatamente da função.
 */
void enterprise_receive_operation(struct operation *op, int enterp_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        // faz wait dos semáforos que controlam o acesso para leitura do buffer entre o intermediário e a empresa
        consume_begin(sems->interm_enterp);
        read_interm_enterp_buffer(buffers->interm_enterp, enterp_id, data->buffers_size, op);
        // faz post dos semáforos que controlam o acesso para leitura do buffer entre o intermediário e a empresa
        consume_end(sems->interm_enterp);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_enterp para
 * o id passado como argumento, alterando o estado da mesma para 'E' ou 'A'
 * conforme o número máximo de operações já tiver sido atingido ou não, e
 * incrementando o contador de operações. Atualiza também a operação na
 * estrutura data.
 */
void enterprise_process_operation(struct operation *op, int enterp_id, struct main_data *data, int *counter, struct semaphores *sems)
{
    // faz wait do semáforo de acesso a data
    semaphore_mutex_lock(sems->results_mutex);

    // escreve o tempo atual em que a empresa processa a operação
    get_time(&op->enterp_time);

    op->receiving_enterp = enterp_id;

    // conta quantas operações já foram executadas no total pois o op->id é inicializado com o número da última operação - 1 na função create-request de main.c.
    int op_counter = op->id + 1;

    // se o numéro máximo de operações for ultrapassado, o estado da operação passa a ser 'A'(agendada)
    if (data->max_ops < op_counter)
    {
        op->status = 'A';
    }
    else // caso contrário, o estado da operação passa a ser 'E'(executado)
    {
        op->status = 'E';
        (*counter)++;
    }
    
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

/* Função principal de uma Empresa. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação e se e data->terminate ainda for igual a 0,
 * processa-a. Operações com id igual a -1 são ignoradas (op inválida) e se
 * data->terminate for igual a 1 é porque foi dada ordem de terminação do
 * programa, portanto deve-se fazer return do número de operações processadas.
 * Para efetuar estes passos, pode usar os outros métodos auxiliares definidos
 * em enterprise.h.
 */
int execute_enterprise(int enterp_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    int counter = 0;

    // Ignora o sinal de interrupção (SIGINT) para que os processos das empresas não sejam encerrados abruptamente
    signal(SIGINT, SIG_IGN);

    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while (*data->terminate != 1)
    {
        enterprise_receive_operation(op, enterp_id, buffers, data, sems);

        if (*data->terminate == 0)
        {
            // se for a empresa correta, entra no if e prossegue
            if (op->id != -1)
            {
                printf("Empresa recebeu pedido!\n");
                enterprise_process_operation(op, enterp_id, data, &counter, sems);
            }
            else
            {
                // caso contrário(ou seja, não ser a empresa correta) volta a fazer o mesmo que o intermediário, volta a sinalizar que outra empresa pode executar
                // faz post dos semáforos que controlam o acesso para escrita do buffer entre o intermediário e a empresa
                produce_end(sems->interm_enterp);
            }
        }
    }
    destroy_dynamic_memory(op);

    return counter;
}
