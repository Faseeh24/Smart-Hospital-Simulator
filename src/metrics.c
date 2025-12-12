#include "metrics.h"
#include "department.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>

// Record patient arrival
void record_patient_arrival(Patient *patient) {
    if (!patient) return;
    patient->arrival_time = time(NULL);
    log_message(LOG_INFO, "Patient %d arrived at hospital", patient->id);
}

// Record treatment start
void record_treatment_start(Patient *patient, DepartmentType dept) {
    if (!patient) return;
    log_message(LOG_DEBUG, "Patient %d started treatment at %s", 
                patient->id, get_department_name(dept));
}

// Record treatment end
void record_treatment_end(Patient *patient, DepartmentType dept, double treatment_time) {
    if (!patient) return;
    patient->total_treatment_time += treatment_time;
    log_message(LOG_DEBUG, "Patient %d completed treatment at %s (%.2fs)", 
                patient->id, get_department_name(dept), treatment_time);
}

// Record waiting time
void record_waiting_time(Patient *patient, double waiting_time) {
    if (!patient) return;
    patient->total_waiting_time += waiting_time;
}

// Record patient discharge
void record_patient_discharge(Patient *patient) {
    if (!patient) return;
    patient->discharge_time = time(NULL);
    patient->completed = 1;
    log_message(LOG_INFO, "Patient %d discharged from hospital", patient->id);
}

// Calculate global metrics
void calculate_global_metrics(Patient **all_patients, int num_patients, GlobalMetrics *metrics) {
    if (!all_patients || !metrics || num_patients == 0) return;
    
    metrics->total_patients = num_patients;
    metrics->avg_waiting_time = 0.0;
    metrics->avg_treatment_time = 0.0;
    metrics->avg_time_in_system = 0.0;
    
    time_t earliest = all_patients[0]->arrival_time;
    time_t latest = all_patients[0]->discharge_time;
    
    for (int i = 0; i < num_patients; i++) {
        Patient *p = all_patients[i];
        
        metrics->avg_waiting_time += p->total_waiting_time;
        metrics->avg_treatment_time += p->total_treatment_time;
        
        if (p->completed && p->discharge_time > 0) {
            double time_in_system = difftime(p->discharge_time, p->arrival_time);
            metrics->avg_time_in_system += time_in_system;
            
            if (p->arrival_time < earliest) earliest = p->arrival_time;
            if (p->discharge_time > latest) latest = p->discharge_time;
        }
    }
    
    metrics->avg_waiting_time /= num_patients;
    metrics->avg_treatment_time /= num_patients;
    metrics->avg_time_in_system /= num_patients;
    
    metrics->simulation_start = earliest;
    metrics->simulation_end = latest;
    
    double simulation_duration_minutes = difftime(latest, earliest) / 60.0;
    metrics->throughput = (simulation_duration_minutes > 0) ? 
                          num_patients / simulation_duration_minutes : 0.0;
}

// Print individual patient metrics
void print_patient_metrics(Patient *patient) {
    if (!patient) return;
    
    const char *route_names[] = {"Route A", "Route B", "Route C", "Route D"};
    
    char arrival_str[26], discharge_str[26];
    struct tm *tm_info;
    
    tm_info = localtime(&patient->arrival_time);
    strftime(arrival_str, 26, "%H:%M:%S", tm_info);
    
    if (patient->discharge_time > 0) {
        tm_info = localtime(&patient->discharge_time);
        strftime(discharge_str, 26, "%H:%M:%S", tm_info);
    } else {
        strcpy(discharge_str, "In Progress");
    }
    
    double total_time = patient->completed ? 
                       difftime(patient->discharge_time, patient->arrival_time) : 0.0;
    
    printf("│ %-4d │ %-8s │ %-10s │ %-12s │ %8.2f │ %10.2f │ %9.2f │\n",
           patient->id,
           route_names[patient->route_type],
           arrival_str,
           discharge_str,
           patient->total_waiting_time,
           patient->total_treatment_time,
           total_time);
}

// Print global metrics
void print_global_metrics(GlobalMetrics *metrics) {
    if (!metrics) return;
    
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              HOSPITAL SIMULATION STATISTICS                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Total Patients Processed    : %d\n", metrics->total_patients);
    printf("Average Waiting Time        : %.2f seconds\n", metrics->avg_waiting_time);
    printf("Average Treatment Time      : %.2f seconds\n", metrics->avg_treatment_time);
    printf("Average Time in System      : %.2f seconds\n", metrics->avg_time_in_system);
    printf("Throughput                  : %.2f patients/minute\n", metrics->throughput);
    
    double total_duration = difftime(metrics->simulation_end, metrics->simulation_start);
    printf("Total Simulation Duration   : %.2f seconds\n\n", total_duration);
}
