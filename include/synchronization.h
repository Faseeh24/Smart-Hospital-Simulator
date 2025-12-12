#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <pthread.h>
#include <semaphore.h>

// Mutex operations
int init_mutex(pthread_mutex_t *mutex);
int lock_mutex(pthread_mutex_t *mutex);
int unlock_mutex(pthread_mutex_t *mutex);
int destroy_mutex(pthread_mutex_t *mutex);

// Semaphore operations
sem_t* create_semaphore(const char *name, int initial_value);
int wait_semaphore(sem_t *sem);
int post_semaphore(sem_t *sem);
int destroy_semaphore(const char *name);
sem_t* open_semaphore(const char *name);

#endif // SYNCHRONIZATION_H
