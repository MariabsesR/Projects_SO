#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "memory.h"
#include "main.h"
#include "process.h"
#include "synchronization.h"
#include "configuration.h"
#include "log.h"
#include "stats.h"
#include "aptime.h"
#include "apsignal.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

struct main_data *data;
struct comm_buffers *buffers;
struct semaphores *sems;

// contador de operações totais
int *count;

/* Função que lê o argumento da aplicação, nomeadamente o ficheiro de configuração
 * com o número máximo de operações, o tamanho dos buffers de memória partilhada
 * usados para comunicação, e o número de clientes, de intermediários e de
 * empresas, entre outros. Guarda esta informação nos campos apropriados da
 * estrutura main_data.
 */
void main_args(int argc, char *argv[], struct main_data *data)
{
    // vê se o nº de argumentos é o certo
    if (argc == 2)
    {
        // lê do ficheiro, para inicializar os parametros do programa em data
        initialize_config(argv[1], data);
        // cria os ficheiros de log e de estatísticas
        create_log(data->log_filename);
        create_stat(data->statistics_filename);
    }
    else
    {
        printf("Uso: admpor configurationFileName\n");
        printf("Exemplo: ./bin/admpor config.txt\n\n");
        exit(0);
    }
}

/* Função que reserva a memória dinâmica necessária para a execução
 * do AdmPor, nomeadamente para os arrays *_pids e *_stats da estrutura
 * main_data. Para tal, pode ser usada a função create_dynamic_memory.
 */

void create_dynamic_memory_buffers(struct main_data *data)
{
    data->client_pids = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->client_stats = create_dynamic_memory(sizeof(int) * data->n_clients);

    data->intermediary_pids = create_dynamic_memory(sizeof(int) * data->n_intermediaries);
    data->intermediary_stats = create_dynamic_memory(sizeof(int) * data->n_intermediaries);

    data->enterprise_pids = create_dynamic_memory(sizeof(int) * data->n_enterprises);
    data->enterprise_stats = create_dynamic_memory(sizeof(int) * data->n_enterprises);
}

/* Função que reserva a memória partilhada necessária para a execução do
 * AdmPor. É necessário reservar memória partilhada para todos os buffers da
 * estrutura comm_buffers, incluindo os buffers em si e respetivos
 * pointers, assim como para o array data->results e variável data->terminate.
 * Para tal, pode ser usada a função create_shared_memory. O array data->results
 * deve ser limitado pela constante MAX_RESULTS.
 */
void create_shared_memory_buffers(struct main_data *data, struct comm_buffers *buffers)
{
    buffers->main_client = create_shared_memory("main_client", data->buffers_size);
    buffers->main_client->ptrs = create_shared_memory(STR_SHM_MAIN_CLIENT_PTR, sizeof(int) * data->max_ops);
    buffers->main_client->buffer = create_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, sizeof(struct operation) * data->max_ops);

    buffers->client_interm = create_shared_memory("client_interm", data->buffers_size);
    buffers->client_interm->ptrs = create_shared_memory(STR_SHM_CLIENT_INTERM_PTR, sizeof(struct pointers));
    buffers->client_interm->buffer = create_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, sizeof(struct operation) * data->max_ops);

    buffers->interm_enterp = create_shared_memory("interm_enterp", data->buffers_size);
    buffers->interm_enterp->ptrs = create_shared_memory(STR_SHM_INTERM_ENTERP_PTR, sizeof(int) * data->max_ops);
    buffers->interm_enterp->buffer = create_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, sizeof(struct operation) * data->max_ops);

    data->results = create_shared_memory("results", MAX_RESULTS);
    data->terminate = create_shared_memory("terminate", sizeof(int));
}

/* Função que inicia os processos dos clientes, intermediários e
 * empresas. Para tal, pode usar as funções launch_*,
 * guardando os pids resultantes nos arrays respetivos
 * da estrutura data.
 */
void launch_processes(struct comm_buffers *buffers, struct main_data *data,
                      struct semaphores *sems)
{
    for (int i = 0; i < data->n_clients; i++)
    {
        data->client_pids[i] = launch_client(i, buffers, data, sems);
    }

    for (int i = 0; i < data->n_intermediaries; i++)
    {
        data->intermediary_pids[i] = launch_interm(i, buffers, data, sems);
    }

    for (int i = 0; i < data->n_enterprises; i++)
    {
        data->enterprise_pids[i] = launch_enterp(i, buffers, data, sems);
    }
}

/* Cria uma nova operação identificada pelo valor atual de op_counter e com os
 * dados introduzidos pelo utilizador na linha de comandos, escrevendo a mesma
 * no buffer de memória partilhada entre main e clientes. Imprime o id da
 * operação e incrementa o contador de operações op_counter. Não deve criar
 * mais operações para além do tamanho do array data->results.
 */
void create_request(int *op_counter, struct comm_buffers *buffers,
                    struct main_data *data, struct semaphores *sems)
{
    if (*op_counter >= MAX_RESULTS)
    {
        printf("O número máximo de pedidos foi alcançado!\n");
        return;
    }

    int op_client, op_enter, is_clientInt, is_enterInt;

    is_clientInt = scanf("%d", &op_client);
    is_enterInt = scanf("%d", &op_enter);

    // confirma se os nºs dos clientes e das empresas são mesmo nºs
    if (is_clientInt == 0)
    {
        op_client = -1;
        op_enter = -1;
    }
    else if (is_enterInt == 0)
    {
        op_enter = -1;
    }

    // escreve a operação no ficheiro log
    formatStringToLog("op", op_client, op_enter);

    // escreve a informação da operação no buffer de memória partilhada

    // faz wait do semáforo de acesso a data
    semaphore_mutex_lock(sems->results_mutex);

    data->results[*op_counter].id = *op_counter;
    data->results[*op_counter].requesting_client = op_client;
    data->results[*op_counter].requested_enterp = op_enter;
    data->results[*op_counter].status = 'M';
    data->results[*op_counter].receiving_client = -1;
    data->results[*op_counter].receiving_interm = -1;
    data->results[*op_counter].receiving_enterp = -1;

    // escreve o tempo atual de incio da operação
    get_time(&data->results[*op_counter].start_time);

    // escreve a operação no buffer main_client a partir de data
    buffers->main_client->buffer[*op_counter] = data->results[*op_counter];

    // faz post do semáforo de acesso a data
    semaphore_mutex_unlock(sems->results_mutex);

    // vê se o cliente requisitado existe, se sim envia-o para o buffer
    if (data->results[*op_counter].requesting_client <= data->n_clients - 1 && data->results[*op_counter].requesting_client >= 0)
    {
        // faz wait dos semáforos que controlam o acesso para escrita do buffer entre a main e o cliente
        produce_begin(sems->main_client);
        write_main_client_buffer(buffers->main_client, sizeof(data->buffers_size), &data->results[*op_counter]);
        // faz post dos semáforos que controlam o acesso para escrita do buffer entre a main e o cliente
        produce_end(sems->main_client);
    }

    printf("O pedido #%d foi criado!\n", *op_counter);

    // incrementa o nº de operações
    *op_counter += 1;
}

/* Função que lê um id de operação do utilizador e verifica se a mesma é valida.
 * Em caso afirmativo imprime informação da mesma, nomeadamente o seu estado, o
 * id do cliente que fez o pedido, o id da empresa requisitada, e os ids do
 * cliente, intermediário, e empresa que a receberam e processaram.
 */
void read_status(struct main_data *data, struct semaphores *sems)
{

    int status_id;
    int index = -1;

    scanf("%d", &status_id);

    // escreve o status no ficheiro log
    formatStringToLog("status", status_id, -10);

    // verifica se a operação pretendida existe
    for (int i = 0; i < *count; i++)
    {
        if (data->results[i].id == status_id && *count != 0)
        {
            index = i;
            break;
        }
    }

    // verifica se a operação existe
    if (index < *count && index >= 0)
    {
        // se o cliente da operação é válido processa
        if (data->results[index].requesting_client <= data->n_clients - 1 && data->results[index].requesting_client >= 0)
        {
            // se o cliente da operação é válidos e se a empresa da operação é
            // inválidos mostra as estatísticas sem informação da empresa
            // (processadas)
            if (!(data->results[index].requested_enterp <= data->n_enterprises - 1 && data->results[index].requested_enterp >= 0))
            {
                printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d, foi recebido pelo cliente %d, processado pelo intermediário %d!\n", status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp, data->results[index].receiving_client, data->results[index].receiving_interm);
            }
            // caso contrário mostra as estatísticas completas (processadas)
            else
            {
                printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d, foi recebido pelo cliente %d, processado pelo intermediário %d, e tratado pela empresa %d!\n", status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp, data->results[index].receiving_client, data->results[index].receiving_interm, data->results[index].receiving_enterp);
            }
        }
        // caso contrário mostra apenas as estatísticas do pedido de operação
        // sem ser processado
        else
        {
            printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d!\n", status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp);
        }
    }
    else
    {
        printf("Pedido %d ainda não é válido!\n", status_id);
    }
}

/* Função que termina a execução do programa AdmPor. Deve começar por
 * afetar a flag data->terminate com o valor 1. De seguida, e por esta
 * ordem, deve esperar que os processos filho terminem, deve escrever as
 * estatisticas finais do programa, e por fim libertar
 * as zonas de memória partilhada e dinâmica previamente
 * reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
 */
void stop_execution(struct main_data *data, struct comm_buffers *buffers, struct semaphores *sems)
{
    *data->terminate = 1;

    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    closeFileStats();
    closeLog();
    destroy_semaphores(sems);
    destroy_memory_buffers(data, buffers);
    exit(0);
}

/* Função que espera que todos os processos previamente iniciados terminem,
 * incluindo clientes, intermediários e empresas. Para tal, pode usar a função
 * wait_process do process.h.
 */
void wait_processes(struct main_data *data)
{
    for (int i = 0; i < data->n_enterprises; i++)
    {
        data->enterprise_stats[i] = wait_process(data->enterprise_pids[i]);
    }

    for (int i = 0; i < data->n_intermediaries; i++)
    {
        data->intermediary_stats[i] = wait_process(data->intermediary_pids[i]);
    }

    for (int i = 0; i < data->n_clients; i++)
    {
        data->client_stats[i] = wait_process(data->client_pids[i]);
    }
}

/* Função que imprime as estatisticas finais do AdmPor, nomeadamente quantas
 * operações foram processadas por cada cliente, intermediário e empresa.
 */
void write_statistics(struct main_data *data)
{
    printf("Terminando o AdmPor! Imprimindo estatísticas:\n");
    addProcessStatistics(data);
    addRequestStatistics(data, count);
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada
 * previamente reservados na estrutura data.
 */
void destroy_memory_buffers(struct main_data *data,
                            struct comm_buffers *buffers)
{
    destroy_shared_memory("results", data->results, MAX_RESULTS);
    destroy_shared_memory("terminate", data->terminate, sizeof(int));

    destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, sizeof(struct operation) * data->max_ops);
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, sizeof(int) * data->max_ops);
    destroy_shared_memory("main_client", buffers->main_client, data->buffers_size);

    destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, sizeof(struct operation) * data->max_ops);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, sizeof(struct pointers));
    destroy_shared_memory("client_interm", buffers->client_interm, data->buffers_size);

    destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, sizeof(struct operation) * data->max_ops);
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, sizeof(int) * data->max_ops);
    destroy_shared_memory("interm_enterp", buffers->interm_enterp, data->buffers_size);

    destroy_dynamic_memory(buffers);

    destroy_dynamic_memory(data->enterprise_pids);
    destroy_dynamic_memory(data->enterprise_stats);

    destroy_dynamic_memory(data->intermediary_pids);
    destroy_dynamic_memory(data->intermediary_stats);

    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->client_stats);

    destroy_dynamic_memory(data);

    destroy_dynamic_memory(count);
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
 * op - cria uma nova operação, através da função create_request
 * status - verifica o estado de uma operação através da função read_status
 * stop - termina o execução do AdmPor através da função stop_execution
 * help - imprime informação sobre os comandos disponiveis
 */
void user_interaction(struct comm_buffers *buffers, struct main_data *data,
                      struct semaphores *sems)
{

    char command[6];
    printf("Ações disponíveis:\n");
    printf("\top cliente empresa - criar uma nova operação\n");
    printf("\tstatus id - consultar o estado de uma operação\n");
    printf("\tstop - termina a execução do AdmPor.\n");
    printf("\thelp - imprime informação sobre as ações disponíveis.\n");

    while (1)
    {
        printf("Introduzir ação:\n");
        scanf("%s", command);

        if (strcmp(command, "op") == 0)
        {
            create_request(count, buffers, data, sems);
        }
        else if (strcmp(command, "status") == 0)
        {
            read_status(data, sems);
        }
        else if (strcmp(command, "stop") == 0)
        {
            // escreve o stop no ficheiro log
            formatStringToLog("stop", -10, -10);
            stop_execution(data, buffers, sems);
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("Ações disponíveis:\n");
            printf("\top cliente empresa - criar uma nova operação\n");
            printf("\tstatus id - consultar o estado de uma operação\n");
            printf("\tstop - termina a execução do AdmPor.\n");
            printf("\thelp - imprime informação sobre as ações disponíveis.\n");

            // escreve o help no ficheiro log
            formatStringToLog("help", -10, -10);
        }
        else
        {
            printf("Ação não reconhecida, insira 'help' para assistência.\n");
            // escreve qualquer comando inexistente que tenha sido introduzido, no ficheiro log
            formatStringToLog(command, -10, -10);
        }
        // para o print de "Introduzir ação:" ficar imediatamente antes da
        // aquisição de parâmetros, devido aos prints dos processos filhos
        usleep(100000);
    }
}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
 * *_full devem ser inicializados com valor 0, semáforos *_empty com valor
 * igual ao tamanho dos buffers de memória partilhada, e os *_mutex com
 * valor igual a 1. Para tal pode ser usada a função semaphore_create.*/
void create_semaphores(struct main_data *data, struct semaphores *sems)
{

    sems->main_client = create_dynamic_memory(sizeof(struct prodcons));
    sems->client_interm = create_dynamic_memory(sizeof(struct prodcons));
    sems->interm_enterp = create_dynamic_memory(sizeof(struct prodcons));

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);

    sems->main_client->full = semaphore_create(STR_SEM_MAIN_CLIENT_FULL, 0);
    sems->main_client->empty = semaphore_create(STR_SEM_MAIN_CLIENT_EMPTY, data->buffers_size);
    sems->main_client->mutex = semaphore_create(STR_SEM_MAIN_CLIENT_MUTEX, 1);

    sems->client_interm->full = semaphore_create(STR_SEM_CLIENT_INTERM_FULL, 0);
    sems->client_interm->empty = semaphore_create(STR_SEM_CLIENT_INTERM_EMPTY, data->buffers_size);
    sems->client_interm->mutex = semaphore_create(STR_SEM_CLIENT_INTERM_MUTEX, 1);

    sems->interm_enterp->full = semaphore_create(STR_SEM_INTERM_ENTERP_FULL, 0);
    sems->interm_enterp->empty = semaphore_create(STR_SEM_INTERM_ENTERP_EMPTY, data->buffers_size);
    sems->interm_enterp->mutex = semaphore_create(STR_SEM_INTERM_ENTERP_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
 * estes percebam que foi dada ordem de terminação do programa. Para tal,
 * pode ser usada a função produce_end sobre todos os conjuntos de semáforos
 * onde possam estar processos adormecidos e um número de vezes igual ao
 * máximo de processos que possam lá estar.*/
void wakeup_processes(struct main_data *data, struct semaphores *sems)
{

    for (int i = 0; i < data->n_clients; i++)
    {
        produce_end(sems->main_client);
    }

    for (int i = 0; i < data->n_intermediaries; i++)
    {
        produce_end(sems->client_interm);
    }

    for (int i = 0; i < data->n_enterprises; i++)
    {
        produce_end(sems->interm_enterp);
    }
}

/* Função que liberta todos os semáforos da estrutura semaphores. */
void destroy_semaphores(struct semaphores *sems)
{

    semaphore_destroy(STR_SEM_INTERM_ENTERP_MUTEX, sems->interm_enterp->mutex);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_EMPTY, sems->interm_enterp->empty);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_FULL, sems->interm_enterp->full);

    semaphore_destroy(STR_SEM_CLIENT_INTERM_MUTEX, sems->client_interm->mutex);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_EMPTY, sems->client_interm->empty);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_FULL, sems->client_interm->full);

    semaphore_destroy(STR_SEM_MAIN_CLIENT_MUTEX, sems->main_client->mutex);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_EMPTY, sems->main_client->empty);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_FULL, sems->main_client->full);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);

    destroy_dynamic_memory(sems->interm_enterp);
    destroy_dynamic_memory(sems->client_interm);
    destroy_dynamic_memory(sems->main_client);

    destroy_dynamic_memory(sems);
}

int main(int argc, char *argv[])
{
    count = malloc(sizeof(int));
    *count = 0;

    // inicializa as estruturas de dados correspondentes aos buffers de memória e semáforos
    data = create_dynamic_memory(sizeof(struct main_data));
    buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    sems = create_dynamic_memory(sizeof(struct semaphores));

    // executa o código
    main_args(argc, argv, data);

    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);

    // encarregue dos sinais (CTRL+C e Alarme)
    signal_handler();
    user_interaction(buffers, data, sems);

    return 0;
}