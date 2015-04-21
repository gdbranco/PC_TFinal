#include "estruturas.h"
void metro_init(metro_t * metro, unsigned int qtd)
{
  for(unsigned int i=0;i<qtd;i++)
  {
    metro[i].lotacao = (sem_t*) malloc(sizeof(sem_t));
    sem_init(metro[i].lotacao,0,QTD_PESSOA_METRO);
    metro[i].porta   = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(metro[i].porta,NULL);
  }
  return;
}

void metro_destroy(metro_t * metro, unsigned int qtd)
{
  for(unsigned int i=0;i<qtd;i++)
  {
    free(metro[i].lotacao);
    free(metro[i].porta);
  }
  free(metro);
  return;
}

void pessoa_init(pessoa_t *pessoa,unsigned int qtd)
{
  unsigned int seed;
  for(unsigned int i=0;i<qtd;i++)
  {
    seed = time(NULL) + i;
    pessoa[i].destino = rand_r(&seed)%QTD_ESTACOES;
    pessoa[i].corrente = 1;
  }
  return;
}

void pessoa_destroy(pessoa_t *pessoa,unsigned int qtd)
{
  return;
}

void show_pessoa(pessoa_t pessoa)
{
  printf("Pos atual : %d\n", pessoa.corrente);
  printf("Destino : %d\n", pessoa.destino);
  return;
}
