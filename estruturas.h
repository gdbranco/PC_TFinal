#ifndef EST_H
#define EST_H
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
//Definicoes
#define QTD_PESSOA_METRO 3
#define QTD_METROS 1
#define QTD_PESSOAS 100 
#define QTD_ESTACOES 7
#define METRO 0
typedef struct _estacao_t
{
	pthread_mutex_t *trilho;
	int valida;
}estacao_t;
void estacao_init(estacao_t *estacao);
void estacao_destroy(estacao_t *estacao);

typedef struct _metro_t
{
  sem_t *lotacao;
  pthread_mutex_t *porta;
  int corrente;
  int id;
}metro_t;
void metro_init(metro_t * metro);
void metro_destroy(metro_t * metro);

typedef struct _pessoas_t
{
  int destino;
  int corrente;
  int metro;
  int id;
}pessoa_t;

void pessoa_init(pessoa_t * pessoa);
void show_pessoa(pessoa_t pessoa);
void pessoa_novo_destino(pessoa_t *pessoa, int j);
void pessoa_destroy(pessoa_t *pessoa);

#endif
