#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Log levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// Function declarations
int init_logger(const char *filename);
void log_message(LogLevel level, const char *format, ...);
void close_logger();

#endif // LOGGER_H
