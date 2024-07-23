#include "stdio.h"
#include "memory.h"
#include "main.h"

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
void enterprise_receive_operation(struct operation *op, int enterp_id, struct comm_buffers *buffers, struct main_data *data)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        read_interm_enterp_buffer(buffers->interm_enterp, enterp_id, data->buffers_size, op);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_enterp para o id
 * passado como argumento, alterando o estado da mesma para 'E' ou 'A' conforme o número
 * máximo de operações já tiver sido atingido ou não, e incrementando o contador de operações.
 * Atualiza também a operação na estrutura data.
 */
void enterprise_process_operation(struct operation *op, int enterp_id, struct main_data *data, int *counter)
{

    op->receiving_enterp = enterp_id;

    if (data->max_ops == (*counter))
    {
        op->status = 'A';
    }
    else
    {
        op->status = 'E';
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
}

/* Função principal de uma Empresa. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação e se e data->terminate ainda for igual a 0, processa-a.
 * Operações com id igual a -1 são ignoradas (op inválida) e se data->terminate for igual
 * a 1 é porque foi dada ordem de terminação do programa, portanto deve-se fazer return do
 * número de operações processadas. Para efetuar estes passos, pode usar os outros
 * métodos auxiliares definidos em enterprise.h.
 */
int execute_enterprise(int enterp_id, struct comm_buffers *buffers, struct main_data *data)
{
    int counter = 0;
    struct operation *op = create_dynamic_memory(sizeof(struct operation));
    while (*data->terminate != 1)
    {

        enterprise_receive_operation(op, enterp_id, buffers, data);

        if (*data->terminate == 0 && op->id != -1)
        {
            printf("Empresa recebeu pedido!\n");
            enterprise_process_operation(op, enterp_id, data, &counter);
        }
    }
    destroy_dynamic_memory(op);

    return counter;
}
