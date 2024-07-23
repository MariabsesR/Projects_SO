#include <stdio.h>
#include <time.h>
#include <string.h>
#include "aptime.h"
FILE *logFile;

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que  abre, ou cria, para escrita um ficheiro com um dado nome.
 * Se o ficheiro existir, perde-se o conteúdo existente.
 */
void create_log(char *str)
{
  logFile = fopen(str, "w"); // open file in write mode
}

/* Função que utiliza apontador para o ficheiro de log previamente aberto e fecha-o.
 */
void closeLog()
{
  int result = fclose(logFile); // close file
  if (result == -1)
  {
    perror("Error closing File");
  }
}

/* Função recebe um char* com uma operação, e adiciona o instante em que a operação foi feita pelo utilizador, indicando o ano, mês, dia,
 * hora, minuto, segundo e milissegundos e escreve no ficheiro aberto/criado previamente.
 */
void addToLog(char *str)
{
  char currentTime[100];

  // estrutura que guarda o instante em que o comando foi inserido
  struct timespec timeCommand;

  // instante em que o comando foi inserido
  get_time(&timeCommand);

  // formata o tempo
  size_t error = strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", localtime(&timeCommand.tv_sec));

  // se não for possível formatar corretamente o tempo, é lançado um erro
  if (error == 0)
  {
    fprintf(stderr, "Erro ao formatar o tempo.\n");
  }

  char final[200];
  // por fim, introduz o conteúdo no ficheiro
  sprintf(final, "%s.%03ld %s", currentTime, timeCommand.tv_nsec / 1000000, str);

  int result = fputs(final, logFile);

  if (result <= 0)
  {
    printf("Error writing to file\n");
  }

  // permite a escrita no ficheiro em tempo real
  fflush(logFile);
}

/* Função que recebe um char* com um determinado comando, seja ele existente ou não, e dois inteiros que podem ser ou não usados
 * dependendo do comando passado. Isto é, se o comando estiver entre os comandos válidos (op, status, help e stop),
 * os inteiros passados deveram estar em conformidade com as requisições de cada comando. Por exemplo: se command == "op", ambos os inteiros serão
 * usados, já no caso de stop e help, nenhum dos inteiros passados será usado, pelo que o seu valor é irrelevante.
 */
void formatStringToLog(char *command, int arg1, int arg2)
{
  char str[100];

  if (strcmp(command, "op") == 0)
  {
    sprintf(str, "op %d %d\n", arg1, arg2);
    addToLog(str);
  }
  else if (strcmp(command, "status") == 0)
  {
    sprintf(str, "status %d\n", arg1);
    addToLog(str);
  }
  else
  {
    sprintf(str, "%s\n", command);
    addToLog(str);
  }
}
