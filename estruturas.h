#ifndef EST_H
#define EST_H
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
//Definicoes
#define QTD_PESSOA_METRO 10
#define QTD_METROS 4
#define QTD_PESSOAS 100
#define QTD_ESTACOES 7
#define METRO 0
typedef struct _metro_t
{
  sem_t *lotacao;
  pthread_mutex_t *porta;
  int corrente;
}metro_t;

typedef struct _pessoas_t
{
  int destino;
  int corrente;
}pessoa_t;

typedef struct _estrutura_start_t
{
	pessoa_t pessoa;
	long id;
}estrutura_start_t;

void metro_init(metro_t * metro, unsigned int qtd);
void metro_destroy(metro_t * metro, unsigned int qtd);
void pessoa_init(pessoa_t * pessoa,unsigned int qtd);
void show_pessoa(pessoa_t pessoa,long meu_id);
void pessoa_destroy(pessoa_t *pessoa,unsigned int qtd);

#endif
