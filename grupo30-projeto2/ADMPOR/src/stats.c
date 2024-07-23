#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "stats.h"
#include "main.h"
FILE *statFile;

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que abre, ou cria, para escrita um ficheiro com um dado nome. Se o ficheiro existir, perde-se o conteúdo existente.
*/
void create_stat(char *str)
{
    statFile = fopen(str, "w");

    if (statFile == NULL)
    {
        printf("Failed to open file for writing.\n");
        exit(1);
    }
}

/* Função que utiliza apontador para o ficheiro de estatísticas previamente aberto e fecha-o.
*/
void closeFileStats()
{
    int result = fclose(statFile);

    if (result == -1)
    {
        perror("Error closing File");
    }
}

/* Função que escreve na consola e no ficheiro, aberto anteriormente, as estatísticas dos vários pedidos processados 
* pelos clientes, intermediários e empresas.
*/
void addProcessStatistics(struct main_data *data)
{
    char final[200];
    int result = fputs("Estatísticas Processadas:\n", statFile);
    printf("Estatísticas Processadas:\n");

    if (result == EOF)
    {
        printf("Failed to write to file.\n");
        exit(1);
    }

    // escrita dos processos dos clientes
    for (int i = 0; i < data->n_clients; i++)
    {  
        if (data->client_stats[i] > 0)
        {
            sprintf(final, "\tCliente %d recebeu %d pedidos!\n", i, data->client_stats[i]);
            printf("\tCliente %d recebeu %d pedidos!\n", i, data->client_stats[i]);

            int result = fputs(final, statFile);

            if (result <= 0)
            {
                printf("Error writing to file\n");
            }
        }
    }

    // escrita dos processos dos intermediários
    for (int i = 0; i < data->n_intermediaries; i++)
    {
        if (data->intermediary_stats[i] > 0)
        {
            sprintf(final, "\tIntermediário %d preparou %d pedidos!\n", i, data->intermediary_stats[i]);
            printf("\tIntermediário %d preparou %d pedidos!\n", i, data->intermediary_stats[i]);

            int result = fputs(final, statFile);

            if (result <= 0)
            {
                printf("Error writing to file\n");
            }
        }
    }

    // escrita dos processos das empresas
    for (int i = 0; i < data->n_enterprises; i++)
    {
        if (data->enterprise_stats[i] > 0)
        {
            sprintf(final, "\tEmpresa %d executou %d pedidos!\n", i, data->enterprise_stats[i]);
            printf("\tEmpresa %d executou %d pedidos!\n", i, data->enterprise_stats[i]);

            int result = fputs(final, statFile);

            if (result <= 0)
            {
                printf("Error writing to file\n");
            }
        }
    }

    fputs("\n", statFile);
    printf("\n");
}

/* Função que escreve na consola e no ficheiro, aberto anteriormente, as estatísticas das várias operações.
* Incluindo, os tempos “Start time”("Pedido Criado"), “Client time”("Cliente Recebeu"), “Intermediary time”("Intermediário Recebeu") e “Enterprise time”("Empresa Recebeu").
* "Total Time"("Tempo Total") é a diferença entre o “Enterprise time” e o “Start time”
* indicam o ano, mês, dia, hora, minuto, segundo e milissegundos. 
*/
void addRequestStatistics(struct main_data *data, int *count)
{
    int result = fputs("Estatísticas dos Pedidos:\n", statFile);
    printf("Estatísticas dos Pedidos:\n");

    if (result <= 0)
    {
        printf("Error writing to file\n");
    }

    for (int i = 0; i < *count; i++) 
    {
        // Numero da operação
        char text[200];
        sprintf(text, "Pedido: %d\n", i);
        fputs(text, statFile);
        printf("Pedido: %d\n", i);

        // Status
        sprintf(text, "Estado: %c\n", data->results[i].status);
        fputs(text, statFile);
        printf("Estado: %c\n", data->results[i].status);

        // Cliente
        sprintf(text, "ID do Cliente: %d\n", data->results[i].receiving_client);
        fputs(text, statFile);
        printf("ID do Cliente: %d\n", data->results[i].receiving_client);

        // Intermediário
        sprintf(text, "ID do Intermediário: %d\n", data->results[i].receiving_interm);
        fputs(text, statFile);
        printf("ID do Intermediário: %d\n", data->results[i].receiving_interm);

        // Empresa
        sprintf(text, "ID da Empresa: %d\n", data->results[i].receiving_enterp);
        fputs(text, statFile);
        printf("ID da Empresa: %d\n", data->results[i].receiving_enterp);

        // Start time
        char stringTime[100];
        strftime(stringTime, sizeof(stringTime), "%Y-%m-%d %H:%M:%S", localtime(&data->results[i].start_time.tv_sec));
        sprintf(text, "Pedido Criado: %s.%03ld\n", stringTime, data->results[i].start_time.tv_nsec / 1000000);
        fputs(text, statFile);
        printf("Pedido Criado: %s.%03ld\n", stringTime, data->results[i].start_time.tv_nsec / 1000000);

        // Client time
        
        // verifica se o cliente é válido
        if (data->results[i].receiving_client != -1)
        {
            char stringTime1[100];
            strftime(stringTime1, sizeof(stringTime), "%Y-%m-%d %H:%M:%S", localtime(&data->results[i].client_time.tv_sec));
            sprintf(text, "Cliente Recebeu: %s.%03ld\n", stringTime1, data->results[i].client_time.tv_nsec / 1000000);
            fputs(text, statFile);
            printf("Cliente Recebeu: %s.%03ld\n", stringTime1, data->results[i].client_time.tv_nsec / 1000000);
        }

        // Intermediary time

        // verifica se o intermediário é válido
        if (data->results[i].receiving_interm != -1)
        {
            char stringTime2[100];
            strftime(stringTime2, sizeof(stringTime2), "%Y-%m-%d %H:%M:%S", localtime(&data->results[i].intermed_time.tv_sec));
            sprintf(text, "Intermediário Recebeu: %s.%03ld\n", stringTime2, data->results[i].intermed_time.tv_nsec / 1000000);
            fputs(text, statFile);
            printf("Intermediário Recebeu: %s.%03ld\n", stringTime2, data->results[i].intermed_time.tv_nsec / 1000000);
        }

        // Enterprise time

        // verifica se a empresa é válida
        if (data->results[i].receiving_enterp != -1)
        {
            char stringTime3[100];
            strftime(stringTime3, sizeof(stringTime3), "%Y-%m-%d %H:%M:%S", localtime(&data->results[i].enterp_time.tv_sec));
            sprintf(text, "Empresa Recebeu: %s.%03ld\n", stringTime3, data->results[i].enterp_time.tv_nsec / 1000000);
            fputs(text, statFile);
            printf("Empresa Recebeu: %s.%03ld\n", stringTime3, data->results[i].enterp_time.tv_nsec / 1000000);

            // time  total
            sprintf(text, "Tempo Total: %ld.%03ld\n", (data->results[i].enterp_time.tv_sec) - (data->results[i].start_time.tv_sec), (data->results[i].enterp_time.tv_nsec / 1000000) - (data->results[i].start_time.tv_nsec / 1000000));
            fputs(text, statFile);
            printf("Tempo Total: %ld.%03ld\n", (data->results[i].enterp_time.tv_sec) - (data->results[i].start_time.tv_sec), (data->results[i].enterp_time.tv_nsec / 1000000) - (data->results[i].start_time.tv_nsec / 1000000));
        }
        
        fputs("\n",statFile);
        printf("\n");
    }
}