#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "process.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

// contador de operações totais
int *count;

/* Função que lê os argumentos da aplicação, nomeadamente o número
 * máximo de operações, o tamanho dos buffers de memória partilhada
 * usados para comunicação, e o número de clientes, de intermediários e de
 * empresas. Guarda esta informação nos campos apropriados da
 * estrutura main_data.
 */
void main_args(int argc, char *argv[], struct main_data *data)
{
  // ve se o nº de argumentos é o certo
  if (argc == 6)
  {
    // verifica se os argumentos são válidos (são inteiros)
    for (int i = 1; i < argc; i++)
    {
      char *endptr;
      strtol(argv[i], &endptr, 10);
      if (*endptr != '\0' || atoi(argv[i]) <= 0)
      {
        printf("Parâmetros incorretos! Exemplo de uso: ./bin/admpor 10 10 1 1 1\n\n");
        exit(0);
      }
    }
    // inicializa os data com os argsv correspondentes
    data->max_ops = atoi(argv[1]);
    data->buffers_size = atoi(argv[2]);
    data->n_clients = atoi(argv[3]);
    data->n_intermediaries = atoi(argv[4]);
    data->n_enterprises = atoi(argv[5]);
  }
  else
  {
    printf("Uso: admpor max_ops buffers_size n_clients n_intermediaries n_enterprises\n");
    printf("Exemplo: ./bin/admpor 10 10 1 1 1\n\n");
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
void launch_processes(struct comm_buffers *buffers, struct main_data *data)
{
  for (int i = 0; i < data->n_clients; i++)
  {
    data->client_pids[i] = launch_client(i, buffers, data);
  }

  for (int i = 0; i < data->n_intermediaries; i++)
  {
    data->intermediary_pids[i] = launch_interm(i, buffers, data);
  }

  for (int i = 0; i < data->n_enterprises; i++)
  {
    data->enterprise_pids[i] = launch_enterp(i, buffers, data);
  }
}

/* Cria uma nova operação identificada pelo valor atual de op_counter e com os
 * dados introduzidos pelo utilizador na linha de comandos, escrevendo a mesma
 * no buffer de memória partilhada entre main e clientes. Imprime o id da
 * operação e incrementa o contador de operações op_counter. Não deve criar
 * mais operações para além do tamanho do array data->results.
 */
void create_request(int *op_counter, struct comm_buffers *buffers, struct main_data *data)
{
  if (*op_counter >= data->max_ops)
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

  // Escreve a informação da operação no buffer de memória partilhada
  data->results[*op_counter].id = *op_counter;
  data->results[*op_counter].requesting_client = op_client;
  data->results[*op_counter].requested_enterp = op_enter;
  data->results[*op_counter].status = 'M';
  data->results[*op_counter].receiving_client = -1;
  data->results[*op_counter].receiving_interm = -1;
  data->results[*op_counter].receiving_enterp = -1;

  // escreve a operação no buffer main_client a partir de data
  buffers->main_client->buffer[*op_counter] = data->results[*op_counter];

  // vê se o cliente requisitado existe, se sim envia-o para o buffer
  if (data->results[*op_counter].requesting_client <= data->n_clients - 1 && data->results[*op_counter].requesting_client >= 0)
    write_main_client_buffer(buffers->main_client, sizeof(buffers->main_client), &data->results[*op_counter]);

  printf("O pedido #%d foi criado!\n", *op_counter);

  // incrementa o nº de operações
  *op_counter += 1;
}

/* Função que lê um id de operação do utilizador e verifica se a mesma é valida.
 * Em caso afirmativo imprime informação da mesma, nomeadamente o seu estado, o
 * id do cliente que fez o pedido, o id da empresa requisitada, e os ids do cliente,
 * intermediário, e empresa que a receberam e processaram.
 */
void read_status(struct main_data *data)
{

  int status_id;
  int index = -1;

  scanf("%d", &status_id);

  // verifica se a operação pretendida existe
  for (int i = 0; i < data->max_ops; i++)
  {
    if (data->results[i].id == status_id && *count != 0)
    {
      index = i;
      break;
    }
  }

  // verifica se a operação existe
  if (index <= (data->max_ops) && index >= 0)
  {
    // se o cliente da operação é válido processa
    if (data->results[index].requesting_client <= data->n_clients - 1 && data->results[index].requesting_client >= 0)
    {
      // se o cliente da operação é válidos e se a empresa da operação é inválidos mostra as estatísticas sem informação da empresa (processadas)
      if (!(data->results[index].requested_enterp <= data->n_enterprises - 1 && data->results[index].requested_enterp >= 0))
      {
        printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d, foi recebido pelo cliente %d, processado pelo intermediário %d!\n",
               status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp, data->results[index].receiving_client, data->results[index].receiving_interm);
      }
      // caso contrário mostra as estatísticas completas (processadas)
      else
      {
        printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d, foi recebido pelo cliente %d, processado pelo intermediário %d, e tratado pela empresa %d!\n",
               status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp, data->results[index].receiving_client, data->results[index].receiving_interm, data->results[index].receiving_enterp);
      }
    }
    // caso contrário mostra apenas as estatísticas do pedido de operação sem ser processado
    else
    {
      printf("Pedido %d com estado %c requisitado pelo cliente %d à empresa %d!\n", status_id, data->results[index].status, data->results[index].requesting_client, data->results[index].requested_enterp);
    }
  }
  else
    printf("Pedido %d ainda não é válido!\n", status_id);
}

/* Função que termina a execução do programa AdmPor. Deve começar por
 * afetar a flag data->terminate com o valor 1. De seguida, e por esta
 * ordem, deve esperar que os processos filho terminem, deve escrever as
 * estatisticas finais do programa, e por fim libertar
 * as zonas de memória partilhada e dinâmica previamente
 * reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
 */
void stop_execution(struct main_data *data, struct comm_buffers *buffers)
{
  *data->terminate = 1;

  wait_processes(data);
  write_statistics(data);
  destroy_memory_buffers(data, buffers);
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

  for (int i = 0; i < data->n_clients; i++)
  {
    printf("Cliente %d preparou %d pedidos!\n", i, data->client_stats[i]);
  }

  for (int i = 0; i < data->n_intermediaries; i++)
  {
    printf("Intermediário %d entregou %d pedidos!\n", i, data->intermediary_stats[i]);
  }

  for (int i = 0; i < data->n_enterprises; i++)
  {
    printf("Empresa %d recebeu %d pedidos!\n", i, data->enterprise_stats[i]);
  }
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
 * reservados na estrutura data.
 */
void destroy_memory_buffers(struct main_data *data, struct comm_buffers *buffers)
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

void user_interaction(struct comm_buffers *buffers, struct main_data *data)
{

  char command[6];
  int repeat = 1;
  printf("Ações disponíveis:\n");
  printf("\top cliente empresa - criar uma nova operação\n");
  printf("\tstatus id - consultar o estado de uma operação\n");
  printf("\tstop - termina a execução do AdmPor.\n");
  printf("\thelp - imprime informação sobre as ações disponíveis.\n");
  while (repeat == 1)
  {
    printf("Introduzir ação:\n");
    scanf("%s", command);

    if (strcmp(command, "op") == 0)
    {
      create_request(count, buffers, data);
    }
    else if (strcmp(command, "status") == 0)
    {
      read_status(data);
    }
    else if (strcmp(command, "stop") == 0)
    {
      stop_execution(data, buffers);
      break;
    }
    else if (strcmp(command, "help") == 0)
    {
      printf("Ações disponíveis:\n");
      printf("\top cliente empresa - criar uma nova operação\n");
      printf("\tstatus id - consultar o estado de uma operação\n");
      printf("\tstop - termina a execução do AdmPor.\n");
      printf("\thelp - imprime informação sobre as ações disponíveis.\n");
    }
    else
    {
      printf("Ação não reconhecida, insira 'help' para assistência.\n");
    }
    // para o print de "Introduzir ação:" ficar imediatamente antes da aquisição de parâmetros, devido aos prints dos processos filhos
    usleep(100000);
  }
}
int main(int argc, char *argv[])
{
  count = malloc(sizeof(int));
  *count = 0;

  // inicializa as estruturas de dados correspondentes aos buffers de memória
  struct main_data *data = create_dynamic_memory(sizeof(struct main_data));
  struct comm_buffers *buffers = create_dynamic_memory(sizeof(struct comm_buffers));

  // executa o código
  main_args(argc, argv, data);
  create_dynamic_memory_buffers(data);
  create_shared_memory_buffers(data, buffers);

  launch_processes(buffers, data);
  user_interaction(buffers, data);
}