

 Grupo nº: SO-030
 Nome: Maria Rocha, Diogo Silva, Pedro Martins
 Nº: fc58208, fc58186, fc58905

De forma a correr o executável, caso se encontre dentro da pasta ADMPOR, deve utilizar:

./bin/admpor max_ops buffers_size n_clients n_intermediaries n_enterprises

Onde max_ops é o número máximo de pedidos (operações) que podem ser criadas,
buffers_size é o tamanho máximo dos buffers, n_clients é o número máximo de
Clientes, n_intermediaries é o número máximo de Intermediários e n_enterprises é o
número máximo de Empresas.
A nível dos argumentos utilizados ao correr o trabalho, deve utilizar apenas números inteiros maiores que 0.

Caso queira refazer os ficheiros .o e o executável admpor e se encontre dentro da pasta ADMPOR, deve utilizar o make clean e de seguida apenas make na consola.

Existe limitações no trabalho, a nível dos argumentos passados no ficheiro no inicio da sua execução,
Só é possível ter no máximo um intermediário, no caso de haver vários intermediários, pode acontecer que
mais que um receba e tente entregar a mesma encomenda. Este problema demonstra a necessidade de efectuar sincronização entre processos,
que será realizada no 2º fase do projeto. 