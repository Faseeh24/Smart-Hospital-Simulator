#include "synchronization.h"
#include "logger.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

// Initialize mutex
int init_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_init(mutex, NULL) != 0) {
        log_message(LOG_ERROR, "Failed to initialize mutex");
        return -1;
    }
    return 0;
}

// Lock mutex
int lock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) != 0) {
        log_message(LOG_ERROR, "Failed to lock mutex");
        return -1;
    }
    return 0;
}

// Unlock mutex
int unlock_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) != 0) {
        log_message(LOG_ERROR, "Failed to unlock mutex");
        return -1;
    }
    return 0;
}

// Destroy mutex
int destroy_mutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_destroy(mutex) != 0) {
        log_message(LOG_ERROR, "Failed to destroy mutex");
        return -1;
    }
    return 0;
}

// Create named semaphore
sem_t* create_semaphore(const char *name, int initial_value) {
    // First unlink if exists
    sem_unlink(name);
    
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0644, initial_value);
    if (sem == SEM_FAILED) {
        log_message(LOG_ERROR, "Failed to create semaphore %s: %s", name, strerror(errno));
        return NULL;
    }
    
    log_message(LOG_INFO, "Created semaphore %s with value %d", name, initial_value);
    return sem;
}

// Open existing semaphore
sem_t* open_semaphore(const char *name) {
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        log_message(LOG_ERROR, "Failed to open semaphore %s: %s", name, strerror(errno));
        return NULL;
    }
    return sem;
}

// Wait on semaphore (decrement)
int wait_semaphore(sem_t *sem) {
    if (sem_wait(sem) != 0) {
        log_message(LOG_ERROR, "Failed to wait on semaphore: %s", strerror(errno));
        return -1;
    }
    return 0;
}

// Post to semaphore (increment)
int post_semaphore(sem_t *sem) {
    if (sem_post(sem) != 0) {
        log_message(LOG_ERROR, "Failed to post to semaphore: %s", strerror(errno));
        return -1;
    }
    return 0;
}

// Destroy named semaphore
int destroy_semaphore(const char *name) {
    if (sem_unlink(name) != 0) {
        log_message(LOG_ERROR, "Failed to destroy semaphore %s: %s", name, strerror(errno));
        return -1;
    }
    log_message(LOG_INFO, "Destroyed semaphore %s", name);
    return 0;
}
