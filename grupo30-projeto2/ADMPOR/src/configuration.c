#include <stdio.h>
#include <stdlib.h>
#include "configuration.h"
#include "memory.h"
#include "main.h"
#include "synchronization.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

#define MAXLINHA 1024
int alarm_time;

/* Função que recebe o nome do ficheiro de configurações e verifica a validade das variáveis, caso
 * caso estejam válidas inicializa-as na data
 */
void initialize_config(char *config_filename, struct main_data *data)
{
    char linha[MAXLINHA];
    FILE *txtfile;
    int line_count = 0;

    // abertura do ficheiro de argumentos em modo leitura
    txtfile = fopen(config_filename, "r");
    if (txtfile == NULL)
    {
        perror("Ficheiro inexistente");
        exit(1);
    }

    // ler linha a linha até não existirem mais linhas
    while (fgets(linha, MAXLINHA, txtfile) != NULL)
    {
        // faz a verificação para os parâmetros que são números que correspondem às linhas 1,2,3,4,5,8 do ficheiro
        if (line_count < 5 || line_count == 7)
        {

            char *endptr;

            // guarda apenas a parte não numérica
            strtol(linha, &endptr, 10);

            // se o 1º não for '\0' e não for '\n' então está lá algo que não era um número(logo dá erro), o 2º verifica se os args são maiores que 0
            if ((*endptr != '\n' && *endptr != '\0') || atoi(linha) <= 0)
            {
                printf("Parâmetros incorretos! Certifique-se que não tem espaços antes ou após os parâmetros! Exemplo de uso: \nmax_ops //nº máximo de operações\nbuffers_size //tamanho dos buffers\n");
                printf("n_clients //nº de clientes\nn_intermediaries //nº de intermediários\nn_enterprises //nº de empresas\n");
                printf("log_filename //nome do ficheiro de log\nstatistics_filename //nome do ficheiro de estatísticas\nalarm_time //temporização para o alarme\n\n");
                exit(0);
            }
        }
        line_count++;
    }

    // verifica a quantidade de parâmetros
    if (line_count != 8)
    {
        printf("\nParâmetros incorretos! Certifique-se que tem a quantidade certa de parâmetros!\n");
        exit(0);
    }

    // após as verificações, volta ao início do ficheiro e retoma a sua leitura
    rewind(txtfile);

    // incializa as respetivas variáveis com os respetivos parâmetros passados no ficheiro
    fscanf(txtfile, "%d", &data->max_ops);
    fscanf(txtfile, "%d", &data->buffers_size);
    fscanf(txtfile, "%d", &data->n_clients);
    fscanf(txtfile, "%d", &data->n_intermediaries);
    fscanf(txtfile, "%d", &data->n_enterprises);
    fscanf(txtfile, "%s", data->log_filename);
    fscanf(txtfile, "%s", data->statistics_filename);
    fscanf(txtfile, "%d", &data->alarm_time);

    // fecha o ficheiro de argumentos
    fclose(txtfile);
}