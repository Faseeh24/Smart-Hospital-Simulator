#include "shared_memory.h"
#include "logger.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

// Create shared memory segment
int create_shared_memory() {
    int shm_id = shmget(SHM_KEY, sizeof(HospitalState), IPC_CREAT | 0666);
    if (shm_id == -1) {
        log_message(LOG_ERROR, "Failed to create shared memory");
        return -1;
    }
    
    log_message(LOG_INFO, "Shared memory created with ID: %d", shm_id);
    return shm_id;
}

// Attach to shared memory
HospitalState* attach_shared_memory(int shm_id) {
    HospitalState *state = (HospitalState*)shmat(shm_id, NULL, 0);
    if (state == (void*)-1) {
        log_message(LOG_ERROR, "Failed to attach to shared memory");
        return NULL;
    }
    
    return state;
}

// Detach from shared memory
void detach_shared_memory(HospitalState *state) {
    if (state) {
        shmdt(state);
    }
}

// Destroy shared memory
void destroy_shared_memory(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        log_message(LOG_ERROR, "Failed to destroy shared memory");
    } else {
        log_message(LOG_INFO, "Shared memory destroyed");
    }
}

// Initialize hospital state
void init_hospital_state(HospitalState *state) {
    if (!state) return;
    
    state->total_patients = 0;
    state->completed_patients = 0;
    state->patients_in_system = 0;
    
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        state->active_patients[i] = 0;
    }
    
    // Initialize mutex with process-shared attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&state->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    
    log_message(LOG_INFO, "Hospital state initialized in shared memory");
}
