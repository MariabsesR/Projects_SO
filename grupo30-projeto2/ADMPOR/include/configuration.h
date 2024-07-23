#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include "main.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que recebe o nome do ficheiro de configurações e verifica a validade das variáveis, caso
 * caso estejam válidas inicializa-as na data
*/
void initialize_config (char* config_filename, struct main_data *data);

#endif