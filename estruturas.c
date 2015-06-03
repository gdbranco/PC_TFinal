#include"estruturas.h"
//--------------METRO--------------------
void MetroInit(metro_t *metro)
{
	metro->id = 0;
	metro->estacao_atual = -1;
	pthread_mutex_init(&metro->porta,NULL);
	pthread_mutex_init(&metro->atualiza,NULL);
	pthread_cond_init(&metro->dentro,NULL);
	sem_init(&metro->lotacao,0,MAX_LOTACAO);
	sem_init(&metro->avanca,0,1);
	//metro->qtd_pessoas = 0;
}
void MetroDestroy(metro_t *metro)
{
	
}
//---------------------------------------
//--------------PESSOA-------------------
void PessoaInit(pessoa_t *pessoa)
{
	unsigned int seed;
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pessoa[i].id = i;
		do
		{
			seed = time(NULL) + i;
			pessoa[i].estacao_atual = rand_r(&seed) % QTD_ESTACOES;
			seed = time(NULL) + 2*i;
			pessoa[i].estacao_destino = rand_r(&seed) % QTD_ESTACOES;
		}while(pessoa[i].estacao_destino == pessoa[i].estacao_atual);
		pessoa[i].estado = ESTADO_ENTRAR;
	}
}

void PessoaNovoDestino(pessoa_t *pessoa, int j)
{
	unsigned int seed = time(NULL) + j;
	do
	{
		pessoa[j].estacao_destino = rand_r(&seed) % QTD_ESTACOES;
	}while(pessoa[j].estacao_atual == pessoa[j].estacao_destino);
	pessoa[j].estado = ESTADO_ENTRAR;
}

void PessoaDestroy(pessoa_t *pessoas)
{
	free(pessoas);
}

void ShowPessoa(pessoa_t pessoa)
{
  printf("Pessoa[%d].NaEstação[%d].ComDestino[%d]\n", pessoa.id, pessoa.estacao_atual, pessoa.estacao_destino);
}
//---------------------------------------
//--------------ESTACAO------------------
void EstacaoInit(estacao_t * estacao)
{
	for(int i=0; i < QTD_ESTACOES; i++)
	{
		pthread_cond_init(&estacao[i].avisa, NULL);
		estacao[i].id = i;
	}
}

void EstacaoDestroy(estacao_t * estacao)
{
	for(int i=0; i < QTD_ESTACOES; i++)
	{
		pthread_cond_destroy(&estacao[i].avisa);
	}
	free(estacao);
}
//---------------------------------------
