#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "client.h"
#include "intermediary.h"
#include "enterprise.h"
#include "main.h"
#include "synchronization.h"
#include "process.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que espera que um processo termine através da função waitpid.
 * Devolve o retorno do processo, se este tiver terminado normalmente.
 */
int wait_process(int process_id)
{

    int result;
    waitpid(process_id, &result, 0);

    // se o processo acabar normalmente ou por sinal(CTRL+C)
    if (WIFEXITED(result) || WIFSIGNALED(result))
        return WEXITSTATUS(result);
    else
        return -1;
}

/* Função que inicia um novo processo cliente através da função fork do SO. O novo
 * processo irá executar a função execute_client respetiva, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_client(int client_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{

    int pid;
    if ((pid = fork()) == -1)
    {
        perror(0);
        exit(1);
    }
    else if (pid == 0)
    {
        int retorno = execute_client(client_id, buffers, data, sems);
        exit(retorno);
    }
    else
    {
        return pid;
    }
}

/* Função que inicia um novo processo intermediário através da função fork do SO. O novo
 * processo irá executar a função execute_intermediary, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_interm(int interm_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    int pid;
    if ((pid = fork()) == -1)
    {
        perror(0);
        exit(1);
    }
    else if (pid == 0)
    {
        int retorno = execute_intermediary(interm_id, buffers, data, sems);
        exit(retorno);
    }
    else
    {

        return pid;
    }
}

/* Função que inicia um novo processo empresa através da função fork do SO. O novo
 * processo irá executar a função execute_enterprise, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_enterp(int enterp_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    int pid;
    if ((pid = fork()) == -1)
    {
        perror(0);
        exit(1);
    }
    else if (pid == 0)
    {
        int retorno = execute_enterprise(enterp_id, buffers, data, sems);
        exit(retorno);
    }
    else
    {

        return pid;
    }
}
