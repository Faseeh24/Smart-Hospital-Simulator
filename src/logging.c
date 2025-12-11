#include "logging.h"
#include <stdlib.h>
#include <stdarg.h> // For variable arguments (if you want more complex logging)

pthread_mutex_t log_mutex; // Definition of the global mutex

void initialize_logging_mutex() {
    if (pthread_mutex_init(&log_mutex, NULL) != 0) {
        perror("Log Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
}

/**
 * Logs an action to stdout in a thread-safe manner.
 * Format: [TIMESTAMP] [SOURCE] ACTION_MESSAGE
 */
void log_action(const char* source, const char* action_message) {
    // 1. Lock the mutex to ensure exclusive access to stdout/file
    pthread_mutex_lock(&log_mutex);
    
    // 2. Get the current time for the timestamp
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    // Format time as HH:MM:SS
    strftime(buffer, 26, "%H:%M:%S", tm_info); 
    
    // 3. Print the formatted log message
    fprintf(stdout, "[%s] [%s] %s\n", buffer, source, action_message);
    fflush(stdout); // Force immediate write to ensure log order is preserved

    // 4. Unlock the mutex
    pthread_mutex_unlock(&log_mutex);
}