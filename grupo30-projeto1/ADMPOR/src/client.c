#include "stdio.h"
#include "client.h"
#include "memory.h"
#include "main.h"

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
int execute_client(int client_id, struct comm_buffers *buffers, struct main_data *data)
{
    int counter = 0;

    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while (*data->terminate != 1)
    {

        client_get_operation(op, client_id, buffers, data);

        if (*data->terminate == 0 && op->id != -1)
        {
            printf("Cliente recebeu pedido!\n");

            client_process_operation(op, client_id, data, &counter);
            client_send_operation(op, buffers, data);
        }
    }

    destroy_dynamic_memory(op);
    return counter;
}

/* Função que lê uma operação do buffer de memória partilhada entre a main
 * e clientes que seja direcionada a client_id. Antes de tentar ler a operação, deve
 * verificar se data->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
 */
void client_get_operation(struct operation *op, int client_id, struct comm_buffers *buffers, struct main_data *data)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        read_main_client_buffer(buffers->main_client, client_id, data->buffers_size, op);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_client para o id
 * passado como argumento, alterando o estado da mesma para 'C' (client), e
 * incrementando o contador de operações. Atualiza também a operação na estrutura data.
 */
void client_process_operation(struct operation *op, int client_id, struct main_data *data, int *counter)
{
    op->receiving_client = client_id;
    op->status = 'C';
    (*counter)++;

    for (int i = 0; i < data->max_ops; i++)
    {
        if (data->results[i].id == op->id)
        {
            data->results[i] = *op;
            break;
        }
    }
}

/* Função que escreve uma operação no buffer de memória partilhada entre
 * clientes e intermediários.
 */
void client_send_operation(struct operation *op, struct comm_buffers *buffers, struct main_data *data)
{

    write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
}