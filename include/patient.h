#ifndef PATIENT_H
#define PATIENT_H

#include "hospital.h"
#include <time.h>

// Patient structure
typedef struct Patient {
    int id;
    RouteType route_type;
    int current_dept_index;  // Index in route array
    time_t arrival_time;
    time_t discharge_time;
    double total_waiting_time;
    double total_treatment_time;
    int completed;
} Patient;

// Patient node for linked list
typedef struct PatientNode {
    Patient *patient;
    struct PatientNode *next;
} PatientNode;

// Function declarations
Patient* create_patient(int id, RouteType route_type);
PatientNode* create_patient_node(Patient *patient);
void add_patient_to_list(PatientNode **head, Patient *patient);
Patient* remove_patient_from_list(PatientNode **head, int patient_id);
void free_patient_list(PatientNode *head);
void print_patient_info(Patient *patient);
DepartmentType get_next_department(Patient *patient);
int is_patient_route_complete(Patient *patient);

#endif // PATIENT_H
