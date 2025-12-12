#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize logger
int init_logger(const char *filename) {
    pthread_mutex_lock(&log_mutex);
    
    log_file = fopen(filename, "w");
    if (!log_file) {
        pthread_mutex_unlock(&log_mutex);
        fprintf(stderr, "Failed to open log file: %s\n", filename);
        return -1;
    }
    
    fprintf(log_file, "=== Hospital Simulation Log ===\n");
    fprintf(log_file, "Started at: %s\n", ctime(&(time_t){time(NULL)}));
    fflush(log_file);
    
    pthread_mutex_unlock(&log_mutex);
    return 0;
}

// Log message with timestamp and level
void log_message(LogLevel level, const char *format, ...) {
    pthread_mutex_lock(&log_mutex);
    
    if (!log_file) {
        pthread_mutex_unlock(&log_mutex);
        return;
    }
    
    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buffer[26];
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Log level strings
    const char *level_strings[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    
    // Write timestamp and level
    fprintf(log_file, "[%s] [%s] ", time_buffer, level_strings[level]);
    
    // Write formatted message
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fflush(log_file);
    
    pthread_mutex_unlock(&log_mutex);
}

// Close logger
void close_logger() {
    pthread_mutex_lock(&log_mutex);
    
    if (log_file) {
        fprintf(log_file, "\n=== Simulation Ended ===\n");
        fclose(log_file);
        log_file = NULL;
    }
    
    pthread_mutex_unlock(&log_mutex);
}
