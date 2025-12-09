#ifndef TYPES_H
#define TYPES_H

#define MAX_DOCTORS 5
#define MAX_BEDS 10
#define MAX_EQUIP 5

typedef struct {
    int patient_id;
    int severity;   // 0 = normal, 1 = urgent, 2 = critical
    int required_time; // treatment time
} Patient;

typedef struct {
    int occupied;
    int patient_id;
} Bed;

typedef struct {
    int doctor_busy[MAX_DOCTORS];
    int beds_available;
    Bed beds[MAX_BEDS];
} HospitalState;

#endif

