#include "hospital.h"
#include "patient.h"
#include "department.h"
#include "scheduler.h"
#include "shared_memory.h"
#include "message_queue.h"
#include "synchronization.h"
#include "logger.h"
#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

// Global variables for cleanup
int shm_id = -1;
int msg_queue_id = -1;
pid_t dept_pids[NUM_DEPARTMENTS];

// Signal handler for cleanup
void cleanup_handler(int signum) {
    (void)signum;  // Unused parameter
    printf("\n\nCleaning up resources...\n");
    
    // Kill all department processes
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        if (dept_pids[i] > 0) {
            kill(dept_pids[i], SIGTERM);
        }
    }
    
    // Destroy IPC resources
    if (msg_queue_id != -1) {
        destroy_message_queue(msg_queue_id);
    }
    
    if (shm_id != -1) {
        destroy_shared_memory(shm_id);
    }
    
    // Destroy semaphores
    destroy_semaphore(SEM_EMERGENCY);
    destroy_semaphore(SEM_OPD);
    destroy_semaphore(SEM_RADIOLOGY);
    destroy_semaphore(SEM_PHARMACY);
    destroy_semaphore(SEM_BILLING);
    
    close_logger();
    
    exit(0);
}

int main() {
    // Register signal handler
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize logger
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║          SMART HOSPITAL SIMULATOR - STARTING...               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    if (init_logger(LOG_FILE) != 0) {
        fprintf(stderr, "Failed to initialize logger\n");
        return 1;
    }
    
    log_message(LOG_INFO, "=== Smart Hospital Simulator Started ===");
    
    // Initialize department configurations
    init_department_configs();
    
    // Create shared memory
    shm_id = create_shared_memory();
    if (shm_id == -1) {
        fprintf(stderr, "Failed to create shared memory\n");
        return 1;
    }
    
    HospitalState *hospital_state = attach_shared_memory(shm_id);
    if (!hospital_state) {
        fprintf(stderr, "Failed to attach to shared memory\n");
        return 1;
    }
    
    init_hospital_state(hospital_state);
    
    // Create message queue
    msg_queue_id = create_message_queue(MSG_QUEUE_BASE_KEY);
    if (msg_queue_id == -1) {
        fprintf(stderr, "Failed to create message queue\n");
        detach_shared_memory(hospital_state);
        return 1;
    }
    
    // Create semaphores for each department
    sem_t *sem_emergency = create_semaphore(SEM_EMERGENCY, EMERGENCY_DOCTORS);
    sem_t *sem_opd = create_semaphore(SEM_OPD, OPD_DOCTORS);
    sem_t *sem_radiology = create_semaphore(SEM_RADIOLOGY, RADIOLOGY_MACHINES);
    sem_t *sem_pharmacy = create_semaphore(SEM_PHARMACY, PHARMACY_PHARMACISTS);
    sem_t *sem_billing = create_semaphore(SEM_BILLING, BILLING_CASHIERS);
    
    if (!sem_emergency || !sem_opd || !sem_radiology || !sem_pharmacy || !sem_billing) {
        fprintf(stderr, "Failed to create semaphores\n");
        detach_shared_memory(hospital_state);
        return 1;
    }
    
    printf("✓ Shared memory created\n");
    printf("✓ Message queue created\n");
    printf("✓ Semaphores created:\n");
    printf("  - Emergency: %d doctors\n", EMERGENCY_DOCTORS);
    printf("  - OPD: %d doctors\n", OPD_DOCTORS);
    printf("  - Radiology: %d machine\n", RADIOLOGY_MACHINES);
    printf("  - Pharmacy: %d pharmacists\n", PHARMACY_PHARMACISTS);
    printf("  - Billing: %d cashier\n\n", BILLING_CASHIERS);
    
    // Fork department processes
    printf("Starting department processes...\n");
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process - department
            department_process((DepartmentType)i);
            exit(0);  // Should never reach here
        } else if (pid > 0) {
            // Parent process
            dept_pids[i] = pid;
            printf("✓ %s department started (PID: %d)\n", 
                   get_department_name((DepartmentType)i), pid);
        } else {
            log_message(LOG_ERROR, "Failed to fork department process");
            cleanup_handler(0);
            return 1;
        }
    }
    
    sleep(1);  // Give departments time to initialize
    
    // Create patients
    printf("\nCreating patients...\n");
    int num_patients = 12;
    Patient **all_patients = (Patient**)malloc(sizeof(Patient*) * num_patients);
    
    // Create diverse patient mix
    all_patients[0] = create_patient(1, ROUTE_A);   // OPD
    all_patients[1] = create_patient(2, ROUTE_A);   // OPD
    all_patients[2] = create_patient(3, ROUTE_A);   // OPD
    all_patients[3] = create_patient(4, ROUTE_B);   // Emergency
    all_patients[4] = create_patient(5, ROUTE_B);   // Emergency
    all_patients[5] = create_patient(6, ROUTE_C);   // Radiology
    all_patients[6] = create_patient(7, ROUTE_C);   // Radiology
    all_patients[7] = create_patient(8, ROUTE_D);   // Pharmacy only
    all_patients[8] = create_patient(9, ROUTE_D);   // Pharmacy only
    all_patients[9] = create_patient(10, ROUTE_A);  // OPD
    all_patients[10] = create_patient(11, ROUTE_B); // Emergency
    all_patients[11] = create_patient(12, ROUTE_D); // Pharmacy only
    
    const char *route_names[] = {"Route A (OPD)", "Route B (Emergency)", 
                                  "Route C (Radiology)", "Route D (Pharmacy)"};
    
    for (int i = 0; i < num_patients; i++) {
        if (all_patients[i]) {
            record_patient_arrival(all_patients[i]);
            printf("  Patient %2d: %s\n", all_patients[i]->id, 
                   route_names[all_patients[i]->route_type]);
        }
    }
    
    hospital_state->total_patients = num_patients;
    
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                  SIMULATION RUNNING...                         ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    // Send initial patients to their first departments using FCFS
    printf("Dispatching patients to departments...\n\n");
    for (int i = 0; i < num_patients; i++) {
        if (all_patients[i]) {
            DepartmentType first_dept = get_next_department(all_patients[i]);
            send_message_to_department(msg_queue_id, first_dept, all_patients[i]);
            all_patients[i]->current_dept_index++;
            usleep(100000);  // Small delay between dispatches
        }
    }
    
    // Run Round Robin message scheduler
    round_robin_message_scheduler(msg_queue_id, all_patients, num_patients);
    
    // Wait for all patients to complete
    printf("\nWaiting for all patients to complete treatment...\n");
    sleep(2);
    
    // Display results
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    PATIENT JOURNEY REPORT                      ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌──────┬──────────┬────────────┬──────────────┬──────────┬────────────┬───────────┐\n");
    printf("│  ID  │  Route   │  Arrival   │  Discharge   │ Waiting  │ Treatment  │   Total   │\n");
    printf("│      │          │    Time    │     Time     │   (s)    │    (s)     │    (s)    │\n");
    printf("├──────┼──────────┼────────────┼──────────────┼──────────┼────────────┼───────────┤\n");
    
    for (int i = 0; i < num_patients; i++) {
        print_patient_metrics(all_patients[i]);
    }
    
    printf("└──────┴──────────┴────────────┴──────────────┴──────────┴────────────┴───────────┘\n");
    
    // Calculate and display global metrics
    GlobalMetrics metrics;
    calculate_global_metrics(all_patients, num_patients, &metrics);
    print_global_metrics(&metrics);
    
    // Display shared memory state
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                 FINAL HOSPITAL STATE                           ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    lock_mutex(&hospital_state->mutex);
    printf("Total Patients          : %d\n", hospital_state->total_patients);
    printf("Completed Patients      : %d\n", num_patients);
    printf("Active Patients:\n");
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        printf("  - %-15s: %d\n", get_department_name((DepartmentType)i),
               hospital_state->active_patients[i]);
    }
    unlock_mutex(&hospital_state->mutex);
    
    printf("\n✓ Simulation completed successfully!\n");
    printf("✓ Log file saved: %s\n\n", LOG_FILE);
    
    // Cleanup
    for (int i = 0; i < num_patients; i++) {
        free(all_patients[i]);
    }
    free(all_patients);
    
    detach_shared_memory(hospital_state);
    
    // Cleanup and exit
    cleanup_handler(0);
    
    return 0;
}
