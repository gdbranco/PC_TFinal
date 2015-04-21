#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t *metro;
pessoa_t *pessoas;
void * start(void * id);
int main()
{
  long i;
  //Alocação de memoria
  metro = (metro_t*) malloc(QTD_METROS*sizeof(metro_t));  
  pessoas = (pessoa_t*) malloc(QTD_PESSOAS*sizeof(pessoa_t));
  pthread_t *handler;
  handler = (pthread_t*)malloc(QTD_PESSOAS*sizeof(pthread_t));
  
  //Inicialização basica
  metro_init(metro,QTD_METROS);
  pessoa_init(pessoas,QTD_PESSOAS);
  
	  estrutura_start_t *go;
  //Programa em si
  for(i=0;i<QTD_PESSOAS;i++)
  {
	  go = (estrutura_start_t*)malloc(sizeof(estrutura_start_t));
	  (*go).id = i;
	  (*go).pessoa = pessoas[i];
    pthread_create(&handler[i],NULL,&start,(void*)go);
  }
  for(i=0;i<QTD_PESSOAS;i++)
  {
    pthread_join(handler[i],NULL);
  }
  
  //Liberando a memoria
  pessoa_destroy(pessoas,QTD_PESSOAS);
  metro_destroy(metro,QTD_METROS);
  free(handler);
  pthread_exit(NULL);
  return 0;
}

void * start(void *id)
{
	estrutura_start_t *meu = (estrutura_start_t*)id;
	printf("Thread %ld\n",meu->id);
  show_pessoa(meu->pessoa);
  free(meu);
  return 0;
}
