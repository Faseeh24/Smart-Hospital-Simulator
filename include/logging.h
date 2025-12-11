#ifndef LOGGING_H
#define LOGGING_H

#include <pthread.h>
#include <time.h>
#include <stdio.h>

// Global lock for all logging operations
extern pthread_mutex_t log_mutex;

// Function to initialize the logging mutex
void initialize_logging_mutex();

// The thread-safe logging function
void log_action(const char* source, const char* action_message);

#endif // LOGGING_H