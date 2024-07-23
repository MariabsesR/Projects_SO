#ifndef LOG_H_GUARD
#define LOG_H_GUARD

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que  abre, ou cria, para escrita um ficheiro com um dado nome.
 * Se o ficheiro existir, perde-se o conteúdo existente.
 */
void create_log(char *log_filename);

/* Função que utiliza apontador para o ficheiro de log previamente aberto e fecha-o.
 */
void closeLog();

/* Função recebe um char* com uma operação, e adiciona o instante em que a operação foi feita pelo utilizador, indicando o ano, mês, dia,
 * hora, minuto, segundo e milissegundos e escreve no ficheiro aberto/criado previamente.
 */
void addToLog(char *str);

/* Função que recebe um char* com um determinado comando, seja ele existente ou não, e dois inteiros que podem ser ou não usados
 * dependendo do comando passado. Isto é, se o comando estiver entre os comandos válidos (op, status, help e stop),
 * os inteiros passados deveram estar em conformidade com as requisições de cada comando. Por exemplo: se command == "op", ambos os inteiros serão
 * usados, já no caso de stop e help, nenhum dos inteiros passados será usado, pelo que o seu valor é irrelevante.
 */
void formatStringToLog(char *command, int arg1, int arg2);
#endif