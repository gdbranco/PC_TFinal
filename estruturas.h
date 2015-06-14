#ifndef EST_H
#define EST_H
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define ESTADO_ENTRAR 0
#define ESTADO_SAIR 1
#define ESTADO_FUNCIONANDO 0
#define ESTADO_QUEBRADO 1
#define MAX_LOTACAO 4
#define QTD_PESSOAS 100
#define QTD_ESTACOES 5
#define QTD_METROS 2
#define METRO QTD_ESTACOES
#define TEMPO_ESPERA_METRO 5
#define TEMPO_VIAGEM 7
#define TEMPO_ESPERA_PESSOA 3
#define TEMPO_CONSERTAR 10
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
	unsigned int estado;
}metro_t;

void MetroInit(metro_t *metro);
void MetroDestroy(metro_t *metro);

typedef struct _pessoa
{
	unsigned int id;
	unsigned int estacao_destino;
	unsigned int estacao_atual;
	unsigned int estado;
	int meu_metro;
}pessoa_t;

void PessoaInit(pessoa_t *pessoa);
void ShowPessoa(pessoa_t pessoa);
void PessoaNovoDestino(pessoa_t *pessoa, int j);
void PessoaDestroy(pessoa_t *pessoa);

typedef struct _estacao
{
	pthread_cond_t avisa;
	unsigned int id;
	int metro_estacao;
	pthread_mutex_t hold;
}estacao_t;

void EstacaoInit(estacao_t *estacao);
void EstacaoDestroy(estacao_t *estacao);

#endif
