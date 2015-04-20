#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"estruturas.h"

metro_t *metro;
pessoa_t *pessoas;
void * func(void * id);
int main()
{
  long i;
  //Alocação de memoria
  metro = (metro_t*) malloc(QTD_METROS*sizeof(metro_t));  
  pessoas = (pessoa_t*) malloc(QTD_PESSOAS*sizeof(pessoa_t));
  
  //Inicialização basica
  metro_init(metro,QTD_METROS);
  pessoa_init(pessoas,QTD_PESSOAS);
  
  //Programa em si
  for(i=0;i<QTD_PESSOAS;i++)
  {
    pthread_create(pessoas[i].eu,NULL,&func,(void*)i);
  }
  for(i=0;i<QTD_PESSOAS;i++)
  {
    pthread_join(*pessoas[i].eu,NULL);
  }
  
  //Liberando a memoria
  pessoa_destroy(pessoas,QTD_PESSOAS);
  metro_destroy(metro,QTD_METROS);
  pthread_exit(NULL);
  return 0;
}

void * func(void *id)
{
  long meu_id = (long)id;
  show_pessoa(
  return;
}