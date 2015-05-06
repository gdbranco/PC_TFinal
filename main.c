#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t *metro;
pessoa_t *pessoas;
pthread_mutex_t l;
estacao_t *estacoes;
sem_t metro_avanca[QTD_METROS];
int qtd_pessoa_metro[QTD_METROS];

void * viagem(void * id);
void * parada(void * id);
int main()
{
	int i;
	//Alocação de memoria
	estacoes = (estacao_t*) malloc(QTD_ESTACOES*sizeof(estacao_t));
	metro = (metro_t*) malloc(QTD_METROS*sizeof(metro_t));  
	pessoas = (pessoa_t*) malloc(QTD_PESSOAS*sizeof(pessoa_t));
	pthread_t *handler;
	handler = (pthread_t*)malloc(QTD_PESSOAS*sizeof(pthread_t));
	pthread_t *metro_handler;
	metro_handler = (pthread_t*) malloc(QTD_METROS*sizeof(pthread_t));

	//Inicialização basica
	metro_init(metro);
	pessoa_init(pessoas);
	pthread_mutex_init(&l,NULL);
	for(i=0;i<QTD_METROS;i++)
	{
		sem_init(&metro_avanca[i],0,1);
		qtd_pessoa_metro[i]=0;
	}
	for(i=0;i<QTD_ESTACOES;i++)
	{
		estacoes[i].valida=-1;
	}

	//Programa em si
	for(i=0;i<QTD_METROS;i++)
	{
		pthread_create(&metro_handler[i],NULL,&viagem,(void*)(intptr_t)i);
	}
	for(i=0;i<QTD_PESSOAS;i++)
	{
		pthread_create(&handler[i],NULL,&parada,(void*)(intptr_t)i);
	}
	for(i=0;i<QTD_METROS;i++)
	{
		pthread_join(metro_handler[i],NULL);
	}
	for(i=0;i<QTD_PESSOAS;i++)
	{
		pthread_join(handler[i],NULL);
	}

	//Liberando a memoria
	pessoa_destroy(pessoas);
	metro_destroy(metro);
	estacao_destroy(estacoes);
	free(handler);
	free(metro_handler);
	pthread_exit(NULL);
	return 0;
}

void * parada(void *id)
{
	int meu_id = (intptr_t)id;
	while(1)
	{
		if(estacoes[pessoas[meu_id].corrente].valida!=-1)
		{
			sem_wait(metro[estacoes[pessoas[meu_id].corrente].valida].lotacao);
			pthread_mutex_lock(metro[estacoes[pessoas[meu_id].corrente].valida].porta);
			qtd_pessoa_metro[estacoes[pessoas[meu_id].corrente].valida]++;
			pessoas[meu_id].metro=estacoes[pessoas[meu_id].corrente].valida;
			pessoas[meu_id].corrente=METRO;
			printf("Pessoa %d entrou no metro %d na estacao %d\n",pessoas[meu_id].id,estacoes[pessoas[meu_id].metro].valida,metro[estacoes[pessoas[meu_id].metro].valida].corrente);
			sleep(1);
			if(qtd_pessoa_metro[estacoes[pessoas[meu_id].metro].valida]==QTD_PESSOA_METRO)
			{
				sem_post(&metro_avanca[estacoes[pessoas[meu_id].metro].valida]);
			}
			pthread_mutex_unlock(metro[estacoes[pessoas[meu_id].metro].valida].porta);
		}
	}
	return 0;
}

void * viagem(void *id)
{
	int meu_id = (intptr_t)id;
	while(1)
	{
		sem_wait(&metro_avanca[meu_id]);
		pthread_mutex_lock(metro[meu_id].porta);
		estacoes[metro[meu_id].corrente].valida = -1;
		metro[meu_id].corrente=(metro[meu_id].corrente+1)%QTD_ESTACOES;
		sleep(1);
		printf("Metro %d chegou na estacao[%d]\n",metro[meu_id].id,metro[meu_id].corrente);
		estacoes[metro[meu_id].corrente].valida = metro[meu_id].id;
		for(int j=0;j<QTD_PESSOAS;j++)
		{
			if(pessoas[j].corrente==METRO && pessoas[j].metro==meu_id)
			{
				if(metro[meu_id].corrente==pessoas[j].destino)
				{	
					qtd_pessoa_metro[meu_id]--;
					printf("Pessoa %d saiu do metro %d na estacao %d\n",pessoas[j].id,meu_id,pessoas[j].destino);
					sleep(2);
					pessoas[j].corrente=metro[meu_id].corrente;
					pessoa_novo_destino(pessoas,j);
					sem_post(metro[meu_id].lotacao);	
				}
			}
		}
		pthread_mutex_unlock(metro[meu_id].porta);
		if(qtd_pessoa_metro[meu_id]==QTD_PESSOA_METRO)
		{
			printf("Metro %d lotado, seguindo viagem\n",meu_id);
			sleep(2);
			sem_post(&metro_avanca[meu_id]);
		}
	}
	return 0;
}
