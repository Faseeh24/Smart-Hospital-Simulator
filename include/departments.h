#ifndef DEPARTMENTS_H
#define DEPARTMENTS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --- 1. Department Enumeration ---
typedef enum {
    EMERGENCY,
    OPD,
    RADIOLOGY,
    PHARMACY,
    BILLING,
    NUM_DEPARTMENTS // A helper to get the count of departments
} DepartmentType;

// --- 2. Department Configuration Structure ---
// Holds the data needed for each department's thread.
typedef struct {
    DepartmentType type;
    char name[30];
    int capacity;         // The maximum number of simultaneous services (doctors/machines)
    sem_t service_sem;    // Semaphore to control the capacity
    pthread_t thread_id;  // The ID of the running department thread
} Department;

// --- 3. Function Prototypes ---

// Initializes the Department array, setting names and semaphore capacity.
void initialize_departments(Department departments[]);

// Generic thread function run by all departments.
void* department_thread_func(void* arg);

// Helper function to get a department's name (for logging)
const char* get_department_name(DepartmentType type);

#endif 