#ifndef STATS_H_GUARD
#define STATS_H_GUARD 
#include "main.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que abre, ou cria, para escrita um ficheiro com um dado nome. Se o ficheiro existir, perde-se o conteúdo existente.
*/
void create_stat(char* str);

/* Função que utiliza apontador para o ficheiro de estatísticas previamente aberto e fecha-o.
*/
void closeFileStats();

/* Função que escreve na consola e no ficheiro, aberto anteriormente, as estatísticas dos vários pedidos processados 
* pelos clientes, intermediários e empresas.
*/
void addProcessStatistics(struct main_data *data);

/* Função que escreve na consola e no ficheiro, aberto anteriormente, as estatísticas das várias operações.
* Incluindo, os tempos “Start time”("Pedido Criado"), “Client time”("Cliente Recebeu"), “Intermediary time”("Intermediário Recebeu") e “Enterprise time”("Empresa Recebeu").
* "Total Time"("Tempo Total") é a diferença entre o “Enterprise time” e o “Start time”
* indicam o ano, mês, dia, hora, minuto, segundo e milissegundos. 
*/
void addRequestStatistics(struct main_data *data, int *count);

#endif