#include "departments.h"

// --- 3. Helper: Get Department Name ---
const char* get_department_name(DepartmentType type) {
    switch (type) {
        case EMERGENCY: return "Emergency";
        case OPD: return "OPD";
        case RADIOLOGY: return "Radiology";
        case PHARMACY: return "Pharmacy";
        case BILLING: return "Billing";
        default: return "UNKNOWN";
    }
}

// --- 4. Initialization ---
void initialize_departments(Department departments[]) {
    // Array of initial capacities (matches the order in DepartmentType enum)
    const int capacities[] = {
        [EMERGENCY] = 2,
        [OPD] = 3,
        [RADIOLOGY] = 1,
        [PHARMACY] = 2,
        [BILLING] = 1
    };

    printf("--- Initializing Departments ---\n");

    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        Department* dept = &departments[i];
        dept->type = (DepartmentType)i;
        dept->capacity = capacities[i];
        
        // Set the name based on the enum
        snprintf(dept->name, sizeof(dept->name), "%s", get_department_name((DepartmentType)i));

        // Initialize Semaphore:
        // arg 1: sem_t* (address of the semaphore)
        // arg 2: pshared (0 for threads, non-zero for processes)
        // arg 3: value (initial count, which is the capacity)
        if (sem_init(&(dept->service_sem), 0, dept->capacity) != 0) {
            perror("Semaphore initialization failed");
            exit(EXIT_FAILURE);
        }
        
        printf("Initialized %s: Capacity=%d (Semaphore Ready)\n", dept->name, dept->capacity);
    }
}

// --- 5. Generic Department Thread Function ---
// This thread is a generic server. Its job is to run indefinitely, 
// wait for patient requests, and dispatch them to the available service slots.
void* department_thread_func(void* arg) {
    // Cast the void* argument back to the Department pointer
    Department* self = (Department*)arg;

    printf("[%s] Department Server running.\n", self->name);

    // The department thread runs indefinitely, waiting for patients to arrive 
    // and use its services.
    while (1) {
        // In a real simulation, this thread would handle incoming patient requests 
        // from a queue (the Bounded Buffer). 
        
        // Placeholder: Wait for patient request (e.g., using a Condition Variable/Queue)
        
        // --- RESOURCE ACQUISITION (Simulated Patient Arrival) ---
        
        // The patient thread (or a dispatcher) would typically handle the sem_wait.
        // For demonstration, we just show the department running and signaling its existence.
        
        // Example logic for a patient thread using this department:
        /* printf("[Patient X] Waiting for service at %s...\n", self->name);
        sem_wait(&(self->service_sem)); // P(V) - Decrement count (Acquire service)
        printf("[Patient X] ACQUIRED service slot at %s. Receiving treatment.\n", self->name);
        
        // Simulate treatment time...
        sleep(2); 
        
        sem_post(&(self->service_sem)); // V() - Increment count (Release service)
        printf("[Patient X] Released service slot at %s.\n", self->name);
        */
        
        // Since the department thread is a persistent server, we'll just have it 
        // log its status and take a small break.
        sleep(5); 
    }

    // Cleanup (though usually not reached)
    sem_destroy(&(self->service_sem));
    return NULL;
}