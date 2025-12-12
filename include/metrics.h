#ifndef METRICS_H
#define METRICS_H

#include "patient.h"
#include <time.h>

// Patient metrics structure
typedef struct {
    int patient_id;
    RouteType route_type;
    time_t arrival_time;
    time_t discharge_time;
    double total_waiting_time;
    double total_treatment_time;
    double time_in_system;
} PatientMetrics;

// Global metrics structure
typedef struct {
    int total_patients;
    double avg_waiting_time;
    double avg_treatment_time;
    double avg_time_in_system;
    double throughput;  // Patients per minute
    time_t simulation_start;
    time_t simulation_end;
} GlobalMetrics;

// Function declarations
void record_patient_arrival(Patient *patient);
void record_treatment_start(Patient *patient, DepartmentType dept);
void record_treatment_end(Patient *patient, DepartmentType dept, double treatment_time);
void record_waiting_time(Patient *patient, double waiting_time);
void record_patient_discharge(Patient *patient);
void calculate_global_metrics(Patient **all_patients, int num_patients, GlobalMetrics *metrics);
void print_patient_metrics(Patient *patient);
void print_global_metrics(GlobalMetrics *metrics);

#endif // METRICS_H
