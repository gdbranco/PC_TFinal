#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t *metro;
pessoa_t *pessoas;
estacao_t *estacoes;


void * Viagem(void * id);
void * Parada(void *id);
void sairmetro(int id,int metro_id);
void entrarmetro(int id,int metro_id);

int main()
{
	// Alocação de memoria
	pessoas  = (pessoa_t*) malloc (QTD_PESSOAS * sizeof(pessoa_t));
	estacoes = (estacao_t*) malloc (QTD_ESTACOES * sizeof(estacao_t));
	metro = (metro_t*) malloc (QTD_METROS * sizeof(metro_t));
	pthread_t *pessoaHandler;
	pthread_t *metroHandler;
	pessoaHandler = (pthread_t*) malloc (QTD_PESSOAS * sizeof(pthread_t));
	metroHandler = (pthread_t*) malloc (QTD_METROS * sizeof(pthread_t));
	printf("Memoria alocada\n");

	// Inicialização basica
	MetroInit(metro);
	PessoaInit(pessoas);
	EstacaoInit(estacoes);
	printf("Inicialização concluida\n");

	// Programa em si
	for(int i=0;i<QTD_METROS;i++)
	{
		pthread_create(&metroHandler[i], NULL, &Viagem, (void*)(intptr_t)i);
	}
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pthread_create(&pessoaHandler[i], NULL, &Parada, (void*)(intptr_t)i);
	}
	for(int i=0;i< QTD_METROS;i++)
	{
		pthread_join(metroHandler[i],NULL);
	}
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pthread_join(pessoaHandler[i], NULL);
	}

	// Liberando a memoria
	PessoaDestroy(pessoas);
	MetroDestroy(metro);
	EstacaoDestroy(estacoes);

	free(pessoaHandler);
	free(metroHandler);

	pthread_exit(NULL);
	printf("Memória Liberada\n");
	return 0;
}

void sairmetro(int id, int metro_id)
{
	sem_post(&metro[metro_id].lotacao);
	pthread_mutex_lock(&metro[metro_id].atualiza);
	metro[metro_id].qtd_pessoas--;
	pthread_mutex_unlock(&metro[metro_id].atualiza);
	pessoas[id].estacao_atual = metro[metro_id].estacao_atual;
	pessoas[id].estado = ESTADO_ENTRAR;
}

void entrarmetro(int id,int metro_id)
{
	sem_wait(&metro[metro_id].lotacao);
	pthread_mutex_lock(&metro[metro_id].atualiza);
	metro[metro_id].qtd_pessoas++;
	pthread_mutex_unlock(&metro[metro_id].atualiza);
	pessoas[id].meu_metro = metro_id;
	pessoas[id].estacao_atual = METRO;
	pessoas[id].estado = ESTADO_SAIR;
}

void *Parada(void *id)
{
	int meu_id = (intptr_t)id;
	int estacao_id, metro_id;
	while(1)
	{
		estacao_id = pessoas[meu_id].estacao_atual;
		if(pessoas[meu_id].estacao_atual == METRO)
		{
			metro_id = pessoas[meu_id].meu_metro;
		}
		else
		{
			metro_id = estacoes[estacao_id].metro_estacao;
		}
		switch(pessoas[meu_id].estado)
		{
			case ESTADO_SAIR:
				/*Decide se a pessoa deve esperar dentro do metro até chegar em sua estacao de destino*/
				pthread_mutex_lock(&metro[metro_id].porta);
				while(pessoas[meu_id].estacao_destino!=metro[metro_id].estacao_atual && pessoas[meu_id].estacao_atual == METRO)
				{
					printf("Pessoa %d esperando o metro %d chegar no destino %d\n",pessoas[meu_id].id,metro_id,pessoas[meu_id].estacao_destino);
					pthread_cond_wait(&metro[metro_id].dentro,&metro[metro_id].porta);
				}
				sairmetro(meu_id,metro_id);
				printf(" <<<< Pessoa %d saiu do metro %d na estacao %d\n", pessoas[meu_id].id,metro_id,metro[metro_id].estacao_atual);
				sleep(TEMPO_ESPERA_PESSOA);
				PessoaNovoDestino(pessoas, meu_id);
				pthread_mutex_unlock(&metro[metro_id].porta);
				break;
			case ESTADO_ENTRAR:
				/*Decide se a pessoa deve esperar na estacao ate um metro chegar nela*/
				pthread_mutex_lock(&metro[metro_id].porta);
				while(pessoas[meu_id].estacao_atual!=metro[metro_id].estacao_atual && pessoas[meu_id].estacao_atual != METRO)
				{
					printf("Pessoa %d esperando um metro chegar na estacao %d\n",pessoas[meu_id].id,pessoas[meu_id].estacao_atual);
					pthread_cond_wait(&estacoes[pessoas[meu_id].estacao_atual].avisa,&metro[metro_id].porta);    
				}
				pthread_mutex_unlock(&metro[metro_id].porta);
				entrarmetro(meu_id,metro_id);
				printf(" >>>> Pessoa %d entrou no metro %d na estacao %d com destino %d\n", pessoas[meu_id].id,pessoas[meu_id].meu_metro,metro[metro_id].estacao_atual,pessoas[meu_id].estacao_destino);
				break;
		}
	}
	return 0;
}

void *Viagem(void *id)
{
	int meu_id = (intptr_t)id;
	/*double delta;*/
	int estacao_id;
	while(1)
	{
		pthread_mutex_lock(&metro[meu_id].porta);
		sem_wait(&metro[meu_id].avanca);
		/*gettimeofday(&metro[meu_id].start,NULL);*/
		metro[meu_id].estacao_atual = (metro[meu_id].estacao_atual+1)%QTD_ESTACOES;
		estacao_id = metro[meu_id].estacao_atual;
		pthread_mutex_lock(&estacoes[estacao_id].hold);
		estacoes[estacao_id].metro_estacao = metro[meu_id].id;
		printf("\nMetro %d chegou na estacao %d\n\n",metro[meu_id].id,metro[meu_id].estacao_atual);
		pthread_cond_broadcast(&metro[meu_id].dentro);
		pthread_cond_broadcast(&estacoes[metro[meu_id].estacao_atual].avisa);
		pthread_mutex_unlock(&metro[meu_id].porta);
		sleep(TEMPO_ESPERA_METRO);
		/*do*/
		/*{*/
			/*gettimeofday(&metro[meu_id].end,NULL);*/
			/*delta = ((metro[meu_id].end.tv_sec - metro[meu_id].start.tv_sec) * 1.e6 + metro[meu_id].end.tv_usec - metro[meu_id].start.tv_usec) / 1.e6;*/
		/*}while(delta<TEMPO_ESPERA_METRO);*/
		sem_post(&metro[meu_id].avanca);
		printf("----Metro %d esta em viagem----\n",metro[meu_id].id);
		pthread_mutex_unlock(&estacoes[estacao_id].hold);
		sleep(TEMPO_VIAGEM);//viagem
	}
	return 0;
}


