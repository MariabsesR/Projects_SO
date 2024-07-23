

 Grupo nº: SO-030
 Nome: Maria Rocha, Diogo Silva, Pedro Martins
 Nº: fc58208, fc58186, fc58905

De forma a correr o executável, caso se encontre dentro da pasta ADMPOR, deve utilizar:

./bin/admpor nome_ficheiro_argumentos.txt

Onde nome_ficheiro_argumentos é o nome do ficheiro que contém os argumentos para a execução do programa.
Este ficheiro deve ser escrito no seguinte formato:
----------------------------------------------------------
|max_ops //nº máximo de operações                        |
|buffers_size //tamanho dos buffers                      |    
|n_clients //nº de clientes                              |
|n_intermediaries //nº de intermediários                 |
|n_enterprises //nº de empresas                          |    
|log_filename //nome do ficheiro de log                  |    
|statistics_filename //nome do ficheiro de estatísticas  |    
|alarm_time //temporização para o alarme                 |
----------------------------------------------------------

A nível dos argumentos numéricos utilizados ao correr o trabalho, deve utilizar apenas números inteiros maiores que 0. 
A nível dos argumentos textuais (log_filename e statistics_filename), o ficheiro de log deve ter extensão .log, e o ficheiro de stats deve ter extensão .txt.

Para gerar os ficheiros .o e o executável admpor deve utilizar o comando make.
Caso queira refazer os ficheiros .o e o executável admpor e se encontre dentro da pasta ADMPOR, deve utilizar o make clean e de seguida apenas make na consola.

Nesta fase do projeto passa a ser possível não só encerrar o programa com o comando 'stop', como também, através da funcionalidade que foi implementada de encerrar 
o programa através do comando CTRL+C, que executa de igual modo o fecho do programa.

O ficheiro de log tem como funcionalidade, registar em tempo real os comandos introduzidos na consola, sejam eles válidos ou não. Sendo que esses registos
serão escritos no seguinte formato:

2023-05-19 10:56:38.412 op 0 0
2023-05-19 10:56:39.798 op 1 1
2023-05-19 10:56:42.366 op 6 7
2023-05-19 10:56:45.728 op 87 9
2023-05-19 10:56:47.929 op 34 6
2023-05-19 10:56:49.355 stop

O ficheiro de stats tem como funcionalidade, registar todas as operações efetuadas, incluindo todos os seus detalhes e informações disponíveis, por exemplo,
se a operação não sair da main, apenas é escrito até ao instante em que o pedido foi criado, ou se a aoperação apenas chegar ao intermediário, apenas são escritas as informações
até ao instante em que o intermediário recebeu a operação, não tendo o tempo total da operação, visto que esta não foi processada até ao fim.
Estas informações serão escritas no seguinte formato:

Estatísticas Processadas:
	Cliente 0 recebeu 1 pedidos!
	Cliente 1 recebeu 1 pedidos!
	Cliente 6 recebeu 1 pedidos!
	Intermediário 0 recebeu 1 pedidos!
	Intermediário 1 recebeu 1 pedidos!
	Intermediário 2 recebeu 1 pedidos!
	Empresa 0 executou 1 pedidos!
	Empresa 1 executou 1 pedidos!
	Empresa 7 executou 1 pedidos!

Estatísticas dos Pedidos:
Pedido: 0
Estado: E
ID do Cliente: 0
ID do Intermediário: 0
ID da Empresa: 0
Pedido Criado: 2023-05-19 10:56:38.443
Cliente Recebeu: 2023-05-19 10:56:38.453
Intermediário Recebeu: 2023-05-19 10:56:38.465
Empresa Recebeu: 2023-05-19 10:56:38.476
Tempo Total: 0.033

Pedido: 1
Estado: E
ID do Cliente: 1
ID do Intermediário: 1
ID da Empresa: 1
Pedido Criado: 2023-05-19 10:56:39.799
Cliente Recebeu: 2023-05-19 10:56:39.807
Intermediário Recebeu: 2023-05-19 10:56:39.817
Empresa Recebeu: 2023-05-19 10:56:39.828
Tempo Total: 0.029

.
.
.

Pedido: 3
Estado: M
ID do Cliente: -1
ID do Intermediário: -1
ID da Empresa: -1
Pedido Criado: 2023-05-19 10:56:45.728

.
.
.

Em enterprise.c, na fase de atribuir um estado à operação, 'E' ou 'A', tomamos a decisão de, quando for atribuído o estado 'A', não incrementar o contador de operações,
visto que, as estatísticas pedidas no enunciado pedem apenas as operações que foram executadas ('E'). Posto isto, as operações são na mesma agendadas, contudo, ao imprimir,
por exemplo, "Empresa 0 executou 1 pedidos!", apenas são contadas as operações executadas.

Em aptime.c, a declaração de CLOCK_REALTIME gera o erro "identifier "CLOCK_REALTIME" is undefined", este erro advém de uma questão das permissões fornecidas,
pelo que, este erro não compremete a execução do programa.

Quanto ao acesso aos buffers main_client e interm_enterp, ao haver mais do que um cliente ou mais do que uma empresa, devido ao semáforo que é desbloqueado não ser determinísitco
no acesso ao buffer, uma vez que o buffer é de acesso aleatório, poderá ocorrer o desbloqueio do cliente ou empresa ao qual a mensagem não lhe é destinada, logo não irá ser processada,
nem o cliente ou empresa certos serão desbloqueados. Posto isto, resolvemos o problema sinalizando de novo o semáforo(produce_end) sempre que isto acontece, acabando por eventualmente desbloquear
o cliente ou empresa pretendidos. Este problema não ocorre no acesso ao buffer client_interm visto que o buffer é circular.

Nesta fase do projeto todas as funcionalidades descritas no enunciado do projeto 2, foram implementadas e testadas com sucesso.


