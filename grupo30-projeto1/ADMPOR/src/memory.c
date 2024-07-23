#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include "memory.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que reserva uma zona de memória partilhada com tamanho indicado
 * por size e nome name, preenche essa zona de memória com o valor 0, e
 * retorna um apontador para a mesma. Pode concatenar o resultado da função
 * getuid() a name, para tornar o nome único para o processo.
 */
void *create_shared_memory(char *name, int size)
{

    void *ptr;
    int ret;

    int fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("shm-open");
        exit(1);
    }

    ret = ftruncate(fd, size);
    if (ret == -1)
    {
        perror("shm-ftruncate");
        exit(2);
    }

    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("shm-mmap");
        exit(3);
    }

    return ptr;
}

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
 * por size, preenche essa zona de memória com o valor 0, e retorna um
 * apontador para a mesma.
 */
void *create_dynamic_memory(int size)
{
    void *ptr = malloc(size); // Aloca uma zona de memória dinâmica de tamanho size

    if (ptr != NULL)
    {                         // Verifica se a alocação foi bem sucedida
        memset(ptr, 0, size); // Preenche a zona de memória com o valor 0
    }

    return ptr; // Retorna um ponteiro para a zona de memória alocada
}

/* Função que liberta uma zona de memória dinâmica previamente reservada.
 */
void destroy_shared_memory(char *name, void *ptr, int size)
{

    int ret = munmap(ptr, size);
    if (ret == -1)
    {
        perror("/shm-munmap");
        exit(7);
    }

    ret = shm_unlink(name);
    if (ret == -1)
    {
        perror("/shm-unlink");
        exit(8);
    }
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
 */
void destroy_dynamic_memory(void *ptr)
{
    free(ptr);
}

/* Função que escreve uma operação no buffer de memória partilhada entre a Main
 * e os clientes. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_main_client_buffer(struct rnd_access_buffer *buffer, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 0)
        {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            break;
        }
    }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os clientes
 * e intermediários. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_client_interm_buffer(struct circular_buffer *buffer, int buffer_size, struct operation *op)
{
    if (((buffer->ptrs->in + 1) % buffer_size) != buffer->ptrs->out)
    {
        buffer->buffer[buffer->ptrs->in] = *op;
        buffer->ptrs->in = (buffer->ptrs->in + 1) % buffer_size;
    }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os intermediários
 * e as empresas. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_interm_enterp_buffer(struct rnd_access_buffer *buffer, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 0)
        {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            break;
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre a Main
 * e os clientes, se houver alguma disponível para ler que seja direcionada ao cliente especificado.
 * A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
 * Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_main_client_buffer(struct rnd_access_buffer *buffer, int client_id, int buffer_size, struct operation *op)
{
    int b = 0;
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].requesting_client == client_id)
        {

            *op = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            b = 1;
            break;
        }
    }
    if (!b)
    {
        (*op).id = -1;
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre os clientes e intermediários,
 * se houver alguma disponível para ler (qualquer intermediário pode ler qualquer operação).
 * A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
 * Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_client_interm_buffer(struct circular_buffer *buffer, int buffer_size, struct operation *op)
{

    if (buffer->ptrs->in != buffer->ptrs->out)
    {
        *op = buffer->buffer[buffer->ptrs->out];
        buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
    }
    else
    {
        op->id = -1;
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre os intermediários e as empresas,
 * se houver alguma disponível para ler dirijida à empresa especificada. A leitura deve
 * ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
 * nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_interm_enterp_buffer(struct rnd_access_buffer *buffer, int enterp_id, int buffer_size, struct operation *op)
{
    int b = 0;
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].requested_enterp == enterp_id)
        {

            *op = buffer->buffer[i];
            buffer->ptrs[i] = 0;
            b = 1;
            break;
        }
    }
    if (!b)
    {
        (*op).id = -1;
    }
}