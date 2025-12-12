#include "department.h"
#include "logger.h"
#include "synchronization.h"
#include "message_queue.h"
#include "shared_memory.h"
#include "metrics.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Global department configurations
DepartmentInfo department_configs[NUM_DEPARTMENTS];

// Initialize department configurations
void init_department_configs() {
    // Emergency Department
    department_configs[EMERGENCY].type = EMERGENCY;
    strcpy(department_configs[EMERGENCY].name, "Emergency");
    department_configs[EMERGENCY].resource_count = EMERGENCY_DOCTORS;
    department_configs[EMERGENCY].sem_name = SEM_EMERGENCY;
    
    // OPD Department
    department_configs[OPD].type = OPD;
    strcpy(department_configs[OPD].name, "OPD");
    department_configs[OPD].resource_count = OPD_DOCTORS;
    department_configs[OPD].sem_name = SEM_OPD;
    
    // Radiology Department
    department_configs[RADIOLOGY].type = RADIOLOGY;
    strcpy(department_configs[RADIOLOGY].name, "Radiology");
    department_configs[RADIOLOGY].resource_count = RADIOLOGY_MACHINES;
    department_configs[RADIOLOGY].sem_name = SEM_RADIOLOGY;
    
    // Pharmacy Department
    department_configs[PHARMACY].type = PHARMACY;
    strcpy(department_configs[PHARMACY].name, "Pharmacy");
    department_configs[PHARMACY].resource_count = PHARMACY_PHARMACISTS;
    department_configs[PHARMACY].sem_name = SEM_PHARMACY;
    
    // Billing Department
    department_configs[BILLING].type = BILLING;
    strcpy(department_configs[BILLING].name, "Billing");
    department_configs[BILLING].resource_count = BILLING_CASHIERS;
    department_configs[BILLING].sem_name = SEM_BILLING;
}

// Get department name
const char* get_department_name(DepartmentType type) {
    if (type >= 0 && type < NUM_DEPARTMENTS) {
        return department_configs[type].name;
    }
    return "Unknown";
}

// Get department resource count
int get_department_resources(DepartmentType type) {
    if (type >= 0 && type < NUM_DEPARTMENTS) {
        return department_configs[type].resource_count;
    }
    return 0;
}

// Get department semaphore name
const char* get_department_semaphore_name(DepartmentType type) {
    if (type >= 0 && type < NUM_DEPARTMENTS) {
        return department_configs[type].sem_name;
    }
    return NULL;
}

// Department process - handles patients (runs as separate process after fork)
void department_process(DepartmentType dept_type) {
    log_message(LOG_INFO, "Department %s process started (PID: %d)", 
                get_department_name(dept_type), getpid());
    
    // Get message queue
    int msg_queue_id = msgget(MSG_QUEUE_BASE_KEY, 0666);
    if (msg_queue_id == -1) {
        log_message(LOG_ERROR, "Department %s: Failed to access message queue", 
                    get_department_name(dept_type));
        exit(1);
    }
    
    // Open semaphore for this department
    sem_t *sem = open_semaphore(get_department_semaphore_name(dept_type));
    if (!sem) {
        log_message(LOG_ERROR, "Department %s: Failed to open semaphore", 
                    get_department_name(dept_type));
        exit(1);
    }
    
    // Attach to shared memory
    int shm_id = shmget(SHM_KEY, sizeof(HospitalState), 0666);
    HospitalState *hospital_state = attach_shared_memory(shm_id);
    if (!hospital_state) {
        log_message(LOG_ERROR, "Department %s: Failed to attach shared memory", 
                    get_department_name(dept_type));
        exit(1);
    }
    
    // Process patients continuously
    while (1) {
        Message msg;
        
        // Receive message for this department (blocking)
        if (receive_message_from_department(msg_queue_id, dept_type, &msg, 1) == 0) {
            time_t wait_start = time(NULL);
            
            log_message(LOG_INFO, "Department %s: Patient %d arrived", 
                        get_department_name(dept_type), msg.patient_id);
            
            // Wait for resource availability (FCFS enforced by semaphore)
            wait_semaphore(sem);
            
            time_t treatment_start = time(NULL);
            double waiting_time = difftime(treatment_start, wait_start);
            
            // Update shared memory - patient being treated
            lock_mutex(&hospital_state->mutex);
            hospital_state->active_patients[dept_type]++;
            unlock_mutex(&hospital_state->mutex);
            
            log_message(LOG_INFO, "Department %s: Treating Patient %d (waited %.2fs)", 
                        get_department_name(dept_type), msg.patient_id, waiting_time);
            
            // Simulate treatment (random time between min and max)
            int treatment_duration = TREATMENT_TIME_MIN + 
                                    (rand() % (TREATMENT_TIME_MAX - TREATMENT_TIME_MIN + 1));
            sleep(treatment_duration);
            
            time_t treatment_end = time(NULL);
            double treatment_time = difftime(treatment_end, treatment_start);
            
            log_message(LOG_INFO, "Department %s: Patient %d treatment complete (%.2fs)", 
                        get_department_name(dept_type), msg.patient_id, treatment_time);
            
            // Release resource
            post_semaphore(sem);
            
            // Update shared memory - treatment complete
            lock_mutex(&hospital_state->mutex);
            hospital_state->active_patients[dept_type]--;
            unlock_mutex(&hospital_state->mutex);
            
            // Send completion message back to scheduler (message type = NUM_DEPARTMENTS + 1)
            MessageBuffer response;
            response.mtype = NUM_DEPARTMENTS + 1;
            response.data = msg;
            response.data.sent_time = time(NULL);
            
            if (msgsnd(msg_queue_id, &response, sizeof(Message), 0) == -1) {
                log_message(LOG_ERROR, "Department %s: Failed to send completion message for Patient %d",
                            get_department_name(dept_type), msg.patient_id);
            }
        }
    }
    
    detach_shared_memory(hospital_state);
}
