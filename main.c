#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t *metro;
pessoa_t *pessoas;
estacao_t *estacoes;
sem_t mecanico;


void * controle_metro(void * id);
void * controle_pessoa(void *id);
void * Conserta(void *args);
void sairmetro(int id,int metro_id);
int entrarmetro(int id,int metro_id);

int main()
{
	printf("\tMAX_LOTACAO : %d;\n \
	QTD_PESSOAS : %d;\n \
	QTD_ESTACOES : %d;\n \
	QTD_METROS : %d;\n \
	TEMPO_ESPERA_METRO : %d;\n \
	TEMPO_VIAGEM : %d;\n \
	TEMPO_ESPERA_PESSOA : %d;\n \
	TEMPO_CONSERTAR : %d.\n\n",
	MAX_LOTACAO,
	QTD_PESSOAS,
	QTD_ESTACOES,
	QTD_METROS,
	TEMPO_ESPERA_METRO,
	TEMPO_VIAGEM,
	TEMPO_ESPERA_PESSOA,
	TEMPO_CONSERTAR);

	pthread_t *pessoaHandler;
	pthread_t *metroHandler;
	// Alocação de memoria
	pessoas  = (pessoa_t*) malloc (QTD_PESSOAS * sizeof(pessoa_t));
	estacoes = (estacao_t*) malloc (QTD_ESTACOES * sizeof(estacao_t));
	metro = (metro_t*) malloc (QTD_METROS * sizeof(metro_t));
	pessoaHandler = (pthread_t*) malloc (QTD_PESSOAS * sizeof(pthread_t));
	metroHandler = (pthread_t*) malloc (QTD_METROS * sizeof(pthread_t));
	printf("Memoria alocada\n");

	// Inicialização basica
	MetroInit(metro);
	PessoaInit(pessoas);
	EstacaoInit(estacoes);
	sem_init(&mecanico,0,0);
	printf("Inicialização concluida\n");

	// Programa em si
	for(int i=0;i<QTD_METROS;i++)
	{
		pthread_create(&metroHandler[i], NULL, &controle_metro, (void*)(intptr_t)i);
	}
	for(int i=0; i < QTD_PESSOAS; i++)
	{
		pthread_create(&pessoaHandler[i], NULL, &controle_pessoa, (void*)(intptr_t)i);
	}
	Conserta(NULL);
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

void *Conserta(void *args)
{
	while(1)
	{
		sem_wait(&mecanico);
		printf("\a\tConsertando o metro ... \n");
		sleep(TEMPO_CONSERTAR);
	}
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

int entrarmetro(int id,int metro_id)
{
	if(sem_trywait(&metro[metro_id].lotacao)==0)
	{
		pthread_mutex_lock(&metro[metro_id].atualiza);
		metro[metro_id].qtd_pessoas++;
		pthread_mutex_unlock(&metro[metro_id].atualiza);
		pessoas[id].meu_metro = metro_id;
		pessoas[id].estacao_atual = METRO;
		pessoas[id].estado = ESTADO_SAIR;
		return 0;
	}
	else
		return 1;
	/*sem_wait(&metro[metro_id].lotacao);*/
}

void *controle_pessoa(void *id)
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
		if(metro_id!=-1)
		{
			switch(pessoas[meu_id].estado)
			{
				case ESTADO_SAIR:
					/*Decide se a pessoa deve esperar dentro do metro até chegar em sua estacao de destino*/
					pthread_mutex_lock(&metro[metro_id].porta);
					while(pessoas[meu_id].estacao_destino!=metro[metro_id].estacao_atual && pessoas[meu_id].estacao_atual == METRO)
					{
						printf(" Pessoa %d esperando o metro %d chegar no destino %d [Dentro do Metrô]\n",pessoas[meu_id].id,metro_id,pessoas[meu_id].estacao_destino);
						pthread_cond_wait(&metro[metro_id].dentro,&metro[metro_id].porta);
					}
					pthread_mutex_unlock(&metro[metro_id].porta);
					sairmetro(meu_id,metro_id);
					printf(" <<<< Pessoa %d saiu do metro %d na estacao %d\n", pessoas[meu_id].id,metro_id,metro[metro_id].estacao_atual);
					sleep(TEMPO_ESPERA_PESSOA);
					PessoaNovoDestino(pessoas, meu_id);
					break;
				case ESTADO_ENTRAR:
					/*Decide se a pessoa deve esperar na estacao ate um metro chegar nela*/
					pthread_mutex_lock(&metro[metro_id].porta);
					while(pessoas[meu_id].estacao_atual!=metro[metro_id].estacao_atual && pessoas[meu_id].estacao_atual != METRO)
					{
						//printf("Pessoa %d esperando um metro chegar na estacao %d [Fora do Metrô]\n",pessoas[meu_id].id,pessoas[meu_id].estacao_atual);
						pthread_cond_wait(&estacoes[pessoas[meu_id].estacao_atual].avisa,&metro[metro_id].porta);
					}
					pthread_mutex_unlock(&metro[metro_id].porta);
					if(entrarmetro(meu_id,metro_id)==0)
					{
						printf(" >>>> Pessoa %d entrou no metro %d na estacao %d com destino %d\n", pessoas[meu_id].id,pessoas[meu_id].meu_metro,metro[metro_id].estacao_atual,pessoas[meu_id].estacao_destino);
					}
					break;
			}
		}
	}
	return 0;
}

void *controle_metro(void *id)
{
	int meu_id = (intptr_t)id;
	int estacao_id;
	unsigned int seed = time(NULL);
	unsigned int seed2 = time(NULL)*meu_id;
	while(1)
	{
		switch(metro[meu_id].estado)
		{
			case ESTADO_FUNCIONANDO:
				//Segura as portas do metro para que as pessoas nao possam entrar quando for mudar de estacao
				metro[meu_id].estacao_atual = (metro[meu_id].estacao_atual+1)%QTD_ESTACOES;
				estacao_id = metro[meu_id].estacao_atual;
				//Nao deixa outro metro chegar na estacao se ja existe algum la
				pthread_mutex_lock(&estacoes[estacao_id].hold);
				pthread_mutex_lock(&metro[meu_id].porta);
				estacoes[estacao_id].metro_estacao = metro[meu_id].id;
				printf("\nMetro %d chegou na estacao %d\n\n",metro[meu_id].id,metro[meu_id].estacao_atual);
				//Solta as portas para que pessoas possam utilizar o metro enquanto estiver parado
				pthread_mutex_unlock(&metro[meu_id].porta);
				//Dar os avisos as pessoas, tanto de dentro do metro quanto na estacao que parou
				pthread_cond_broadcast(&metro[meu_id].dentro);
				pthread_cond_broadcast(&estacoes[metro[meu_id].estacao_atual].avisa);
				//Espera um pouco para as pessoas poderem entrar no metro
				sleep(TEMPO_ESPERA_METRO);
				//Segura as portas para comecar a viagem
				pthread_mutex_lock(&metro[meu_id].porta);
				printf("----Metro %d esta em viagem----\n",metro[meu_id].id);
				estacoes[estacao_id].metro_estacao = -1;
				pthread_mutex_unlock(&estacoes[estacao_id].hold);
				sleep(TEMPO_VIAGEM);//viagem
				seed = rand_r(&seed)*rand_r(&seed2);
				if(rand_r(&seed)%100 < 21)
				{
					metro[meu_id].estado = ESTADO_QUEBRADO;
					printf("---|METRO %d QUEBROU|---\n", meu_id);
				}				
				pthread_mutex_unlock(&metro[meu_id].porta);
				break;
			case ESTADO_QUEBRADO: //Nao implementado
				pthread_mutex_lock(&metro[meu_id].porta);
				sem_post(&mecanico);
				sleep(TEMPO_CONSERTAR+1);
				metro[meu_id].estado = ESTADO_FUNCIONANDO;
				printf("---|METRO %d FUNCIONANDO|---\n", meu_id);
				pthread_mutex_unlock(&metro[meu_id].porta);
				break;
		}
	}
	return 0;
}


