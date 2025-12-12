#include "patient.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Route definitions for each patient type
static DepartmentType route_a[] = {OPD, PHARMACY, BILLING};
static DepartmentType route_b[] = {EMERGENCY, RADIOLOGY, PHARMACY, BILLING};
static DepartmentType route_c[] = {RADIOLOGY, OPD, BILLING};
static DepartmentType route_d[] = {PHARMACY, BILLING};

// Create a new patient with dynamic memory allocation
Patient* create_patient(int id, RouteType route_type) {
    Patient *patient = (Patient*)malloc(sizeof(Patient));
    if (!patient) {
        log_message(LOG_ERROR, "Failed to allocate memory for patient %d", id);
        return NULL;
    }
    
    patient->id = id;
    patient->route_type = route_type;
    patient->current_dept_index = 0;
    patient->arrival_time = time(NULL);
    patient->discharge_time = 0;
    patient->total_waiting_time = 0.0;
    patient->total_treatment_time = 0.0;
    patient->completed = 0;
    
    log_message(LOG_INFO, "Created Patient %d with Route Type %d", id, route_type);
    return patient;
}

// Create a patient node for linked list
PatientNode* create_patient_node(Patient *patient) {
    PatientNode *node = (PatientNode*)malloc(sizeof(PatientNode));
    if (!node) {
        log_message(LOG_ERROR, "Failed to allocate memory for patient node");
        return NULL;
    }
    
    node->patient = patient;
    node->next = NULL;
    return node;
}

// Add patient to end of linked list
void add_patient_to_list(PatientNode **head, Patient *patient) {
    PatientNode *new_node = create_patient_node(patient);
    if (!new_node) return;
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        PatientNode *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Remove patient from linked list by ID
Patient* remove_patient_from_list(PatientNode **head, int patient_id) {
    if (*head == NULL) return NULL;
    
    PatientNode *current = *head;
    PatientNode *prev = NULL;
    
    while (current != NULL) {
        if (current->patient->id == patient_id) {
            Patient *patient = current->patient;
            
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            
            free(current);
            return patient;
        }
        prev = current;
        current = current->next;
    }
    
    return NULL;
}

// Free entire patient linked list
void free_patient_list(PatientNode *head) {
    PatientNode *current = head;
    while (current != NULL) {
        PatientNode *next = current->next;
        free(current->patient);
        free(current);
        current = next;
    }
}

// Print patient information
void print_patient_info(Patient *patient) {
    if (!patient) return;
    
    const char *route_names[] = {"Route A (OPD)", "Route B (Emergency)", 
                                  "Route C (Radiology)", "Route D (Pharmacy)"};
    
    printf("Patient ID: %d | Route: %s | Waiting: %.2fs | Treatment: %.2fs\n",
           patient->id, route_names[patient->route_type],
           patient->total_waiting_time, patient->total_treatment_time);
}

// Get next department for patient based on route
DepartmentType get_next_department(Patient *patient) {
    if (!patient) return -1;
    
    DepartmentType *route = NULL;
    int route_length = 0;
    
    switch (patient->route_type) {
        case ROUTE_A:
            route = route_a;
            route_length = sizeof(route_a) / sizeof(DepartmentType);
            break;
        case ROUTE_B:
            route = route_b;
            route_length = sizeof(route_b) / sizeof(DepartmentType);
            break;
        case ROUTE_C:
            route = route_c;
            route_length = sizeof(route_c) / sizeof(DepartmentType);
            break;
        case ROUTE_D:
            route = route_d;
            route_length = sizeof(route_d) / sizeof(DepartmentType);
            break;
        default:
            return -1;
    }
    
    if (patient->current_dept_index >= route_length) {
        return -1;  // Route complete
    }
    
    return route[patient->current_dept_index];
}

// Check if patient route is complete
int is_patient_route_complete(Patient *patient) {
    DepartmentType next = get_next_department(patient);
    return (next == (DepartmentType)-1);
}
