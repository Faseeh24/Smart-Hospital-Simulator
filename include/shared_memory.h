#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "hospital.h"
#include <pthread.h>

// Shared hospital state
typedef struct {
    int total_patients;
    int active_patients[NUM_DEPARTMENTS];
    int completed_patients;
    int patients_in_system;
    pthread_mutex_t mutex;
} HospitalState;

// Function declarations
int create_shared_memory();
HospitalState* attach_shared_memory(int shm_id);
void detach_shared_memory(HospitalState *state);
void destroy_shared_memory(int shm_id);
void init_hospital_state(HospitalState *state);

#endif // SHARED_MEMORY_H
