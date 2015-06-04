#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t metro;
pessoa_t *pessoas;
estacao_t *estacoes;


void * Viagem(void * id);
void * Parada(void *id);
void sairmetro(int id);
void entrarmetro(int id);

int main()
{
	// Alocação de memoria
	pessoas  = (pessoa_t*) malloc (QTD_PESSOAS * sizeof(pessoa_t));
	estacoes = (estacao_t*) malloc (QTD_ESTACOES * sizeof(estacao_t));
	pthread_t *pessoaHandler;
	pthread_t metroHandler;
	pessoaHandler = (pthread_t*) malloc (QTD_PESSOAS * sizeof(pthread_t));
	printf("Memoria alocada\n");

	// Inicialização basica
	MetroInit(&metro);
	PessoaInit(pessoas);
	EstacaoInit(estacoes);
	printf("Inicialização concluida\n");

	// Programa em si
	pthread_create(&metroHandler, NULL, &Viagem, (void*)(intptr_t)0);
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pthread_create(&pessoaHandler[i], NULL, &Parada, (void*)(intptr_t)i);
	}
	pthread_join(metroHandler,NULL);
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pthread_join(pessoaHandler[i], NULL);
	}

	// Liberando a memoria
	PessoaDestroy(pessoas);
	MetroDestroy(&metro);
	EstacaoDestroy(estacoes);

	free(pessoaHandler);

	pthread_exit(NULL);
	printf("Memória Liberada\n");
	return 0;
}

void sairmetro(int id)
{
	sem_post(&metro.lotacao);
	pthread_mutex_lock(&metro.atualiza);
	metro.qtd_pessoas--;
	pthread_mutex_unlock(&metro.atualiza);
	pessoas[id].estacao_atual = metro.estacao_atual;
	pessoas[id].estado = ESTADO_ENTRAR;
}

void entrarmetro(int id)
{
	sem_wait(&metro.lotacao);
	pthread_mutex_lock(&metro.atualiza);
	metro.qtd_pessoas++;
	pthread_mutex_unlock(&metro.atualiza);
	pessoas[id].estacao_atual = METRO;
	pessoas[id].estado = ESTADO_SAIR;
}

void *Parada(void *id)
{
	int meu_id = (intptr_t)id;
	while(1)
	{
		switch(pessoas[meu_id].estado)
		{
			case ESTADO_SAIR:
				/*Decide se a pessoa deve esperar dentro do metro até chegar em sua estacao de destino*/
				pthread_mutex_lock(&metro.porta);
				while(pessoas[meu_id].estacao_destino!=metro.estacao_atual && pessoas[meu_id].estacao_atual == METRO)
				{
					printf("Pessoa %d esperando o metro chegar no destino %d\n",pessoas[meu_id].id,pessoas[meu_id].estacao_destino);
					pthread_cond_wait(&metro.dentro,&metro.porta);
				}
				sairmetro(meu_id);
				printf(" <<<< Pessoa %d saiu do metro na estacao %d\n", pessoas[meu_id].id,metro.estacao_atual);
				sleep(3);
				PessoaNovoDestino(pessoas, meu_id);
				pthread_mutex_unlock(&metro.porta);
				break;
			case ESTADO_ENTRAR:
				/*Decide se a pessoa deve esperar na estacao ate um metro chegar nela*/
				pthread_mutex_lock(&metro.porta);
				while(pessoas[meu_id].estacao_atual!=metro.estacao_atual && pessoas[meu_id].estacao_atual != METRO)
				{
					printf("Pessoa %d esperando o metro chegar na estacao %d\n",pessoas[meu_id].id,pessoas[meu_id].estacao_atual);
					pthread_cond_wait(&estacoes[pessoas[meu_id].estacao_atual].avisa,&metro.porta);    
				}
				pthread_mutex_unlock(&metro.porta);
				entrarmetro(meu_id);
				printf(" >>>> Pessoa %d entrou no metro na estacao %d com destino %d\n", pessoas[meu_id].id,metro.estacao_atual,pessoas[meu_id].estacao_destino);
				break;
		}
	}
	return 0;
}

void *Viagem(void *id)
{
	//int meu_id = (intptr_t)id;
	double delta;
	while(1)
	{
		pthread_mutex_lock(&metro.porta);
		sem_wait(&metro.avanca);
		gettimeofday(&metro.start,NULL);
		metro.estacao_atual = (metro.estacao_atual+1)%QTD_ESTACOES;
		printf("\nMetro chegou na estacao %d\n\n",metro.estacao_atual);
		pthread_cond_broadcast(&metro.dentro);
		pthread_cond_broadcast(&estacoes[metro.estacao_atual].avisa);
		pthread_mutex_unlock(&metro.porta);
		do
		{
			gettimeofday(&metro.end,NULL);
			delta = ((metro.end.tv_sec - metro.start.tv_sec) * 1.e6 + metro.end.tv_usec - metro.start.tv_usec) / 1.e6;
		}while(delta<2);
		sem_post(&metro.avanca);
	}
	return 0;
}


