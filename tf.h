#ifndef METRO_H
#define METRO_H
#include <pthread.h>
#include <semaphore.h>
#define METRO 0
typedef struct _metro_t
{
  sem_t lotacao;
  pthread_mutex_t porta;
}metro_t;

typedef struct _pessoas_t
{
  pthread_t eu;
  int destino;
  int corrente;
}pessoas_t;
#endif