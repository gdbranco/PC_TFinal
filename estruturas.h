#ifndef EST_H
#define EST_H
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define ESTADO_ENTRAR 0
#define ESTADO_SAIR 1
#define MAX_LOTACAO 3
#define QTD_PESSOAS 10
#define QTD_ESTACOES 5
#define QTD_METROS 1
#define METRO QTD_ESTACOES
typedef struct _metro
{
	unsigned int id;
	unsigned int estacao_atual;
	pthread_mutex_t porta;
	pthread_mutex_t atualiza;
	sem_t lotacao;
	sem_t avanca;
	pthread_cond_t dentro;
	unsigned int qtd_pessoas;
	struct timeval start,end;
}metro_t;

void MetroInit(metro_t *metro);
void MetroDestroy(metro_t *metro);

typedef struct _pessoa
{
	unsigned int id;
	unsigned int estacao_destino;
	unsigned int estacao_atual;
	unsigned int estado;
}pessoa_t;

void PessoaInit(pessoa_t *pessoa);
void ShowPessoa(pessoa_t pessoa);
void PessoaNovoDestino(pessoa_t *pessoa, int j);
void PessoaDestroy(pessoa_t *pessoa);

typedef struct _estacao
{
	pthread_cond_t avisa;
	unsigned int id;
}estacao_t;

void EstacaoInit(estacao_t *estacao);
void EstacaoDestroy(estacao_t *estacao);

#endif
