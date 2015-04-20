#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include"tf.h"
#define QTD_PESSOA_METRO 10
#define QTD_METROS 4
#define QTD_PESSOAS 100
metro_t *metro;
pessoas_t *pessoas;
void * func(void * id);

int main()
{
  long i;
  metro = (metro_t*) malloc(QTD_METROS*sizeof(metro_t));
  for(i=0;i<QTD_METROS;i++)
  {
    sem_init(&metro[i].lotacao,0,QTD_PESSOA_METRO);
  }
  pessoas = (pessoas_t*) malloc(QTD_PESSOAS*sizeof(pessoas_t));
  for(i=0;i<QTD_PESSOAS;i++)
  {
    //pessoas[i].eu = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_create(&pessoas[i].eu,NULL,&func,(void*)i);
  }
  for(i=0;i<NUM_PESSOAS;i++)
  {
    //pessoas[i].eu = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_join(pessoas[i].eu,NULL);
  }
  free(metro);
  free(pessoas);
  pthread_exit(NULL);
  return 0;
}

void * func(void *id)
{
  long meu_id = (long)id;
  printf("Pessoa %ld ta pouco se fudendo\n",meu_id);
  return 0;
}