#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include "synchronization.h"

/*
 * Grupo nº: SO-030
 * Nome: Maria Rocha, Diogo Silva, Pedro Martins
 * Nº: fc58208, fc58186, fc58905
 */

/* Função que cria um novo semáforo com nome name e valor inicial igual a
 * value. Pode concatenar o resultado da função getuid() a name, para tornar
 * o nome único para o processo.
 */
sem_t *semaphore_create(char *name, int value)
{

	sem_t *sem;
	sem = sem_open(name, O_CREAT, 0xFFFFFFFF, value);

	if (sem == SEM_FAILED)
	{
		perror("full");
		exit(6);
	}

	return sem;
}

/* Função que destroi o semáforo passado em argumento.
 */
void semaphore_destroy(char *name, sem_t *semaphore)
{

	if (sem_close(semaphore) == -1)
	{
		perror("sem");
	}

	if (sem_unlink(name) == -1)
	{
		perror("sem");
	}
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
 * corretos da estrutura passada em argumento.
 */
void produce_begin(struct prodcons *pc)
{
	if (sem_wait(pc->empty) == -1)
	{

		perror("sem wait");
	}

	if (sem_wait(pc->mutex) == -1)
	{

		perror("sem wait");
	}
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
 * corretos da estrutura passada em argumento.
 */
void produce_end(struct prodcons *pc)
{	
	if (sem_post(pc->mutex) == -1)
	{

		perror("sem post");
	}

	if (sem_post(pc->full) == -1)
	{

		perror("sem post");
	}
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
 * corretos da estrutura passada em argumento.
 */
void consume_begin(struct prodcons *pc)
{
	if (sem_wait(pc->full) == -1)
	{

		perror("sem wait");
	}

	if (sem_wait(pc->mutex) == -1)
	{

		perror("sem wait");
	}
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
 * corretos da estrutura passada em argumento.
 */
void consume_end(struct prodcons *pc)
{
	if (sem_post(pc->mutex) == -1)
	{

		perror("sem post");
	}

	if (sem_post(pc->empty) == -1)
	{

		perror("sem post");
	}
}

/* Função que faz wait a um semáforo.
 */
void semaphore_mutex_lock(sem_t *sem)
{

	if (sem_wait(sem) == -1)
	{

		perror("sem wait");
	}
}

/* Função que faz post a um semáforo.
 */
void semaphore_mutex_unlock(sem_t *sem)
{

	if (sem_post(sem) == -1)
	{

		perror("sem post");
	}
}