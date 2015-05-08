#include "estruturas.h"
void estacao_init(estacao_t * estacao)
{
	for(unsigned int i=0;i<QTD_ESTACOES;i++)
	{
		estacao[i].trilho = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		estacao[i].espera_parada = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
		pthread_mutex_init(estacao[i].trilho,NULL);
		pthread_cond_init(estacao[i].espera_parada,NULL);
		estacao[i].valida=-1;
	}
	return;
}

void estacao_destroy(estacao_t * estacao)
{
	for(unsigned int i=0;i<QTD_ESTACOES;i++)
	{
		pthread_mutex_destroy(estacao[i].trilho);
		free(estacao[i].trilho);
		pthread_cond_destroy(estacao[i].espera_parada);
		free(estacao[i].espera_parada);
	}
	free(estacao);
	return;
}
void metro_init(metro_t * metro)
{
  for(unsigned int i=0;i<QTD_METROS;i++)
  {
    metro[i].lotacao = (sem_t*) malloc(sizeof(sem_t));
    sem_init(metro[i].lotacao,0,QTD_PESSOA_METRO);
    metro[i].porta   = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(metro[i].porta,NULL);
	metro[i].corrente=-1;
	metro[i].id = i;
  }
  return;
}

void metro_destroy(metro_t * metro)
{
  for(unsigned int i=0;i<QTD_METROS;i++)
  {
    free(metro[i].lotacao);
    free(metro[i].porta);
  }
  free(metro);
  return;
}

void pessoa_init(pessoa_t *pessoa)
{
  unsigned int seed;
  for(unsigned int i=0;i<QTD_PESSOAS;i++)
  {
	do
	{
		seed = time(NULL) + i;
		pessoa[i].destino = rand_r(&seed)%QTD_ESTACOES;
		seed = time(NULL)*2 + i;
		pessoa[i].corrente = rand_r(&seed)%QTD_ESTACOES;
	}while(pessoa[i].destino==pessoa[i].corrente);
	pessoa[i].metro = -1;
	pessoa[i].id = i;
  }
  return;
}

void pessoa_novo_destino(pessoa_t *pessoa, int j)
{
	unsigned int seed;
	seed = time(NULL)+j;
	do
	{
		pessoa[j].destino = rand_r(&seed)%QTD_ESTACOES;
	}while(pessoa[j].corrente==pessoa[j].destino);
	return;
}


void pessoa_destroy(pessoa_t *pessoa)
{
  return;
}

void show_pessoa(pessoa_t pessoa)
{
  printf("Pos atual : %d\n", pessoa.corrente);
  printf("Destino : %d\n", pessoa.destino);
  return;
}
