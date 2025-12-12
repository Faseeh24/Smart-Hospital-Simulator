#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <time.h>

// Department types
typedef enum {
    EMERGENCY = 0,
    OPD = 1,
    RADIOLOGY = 2,
    PHARMACY = 3,
    BILLING = 4,
    NUM_DEPARTMENTS = 5
} DepartmentType;

// Patient route types
typedef enum {
    ROUTE_A,  // Normal OPD: OPD → Pharmacy → Billing → Exit
    ROUTE_B,  // Emergency: Emergency → Radiology (optional) → Pharmacy → Billing → Exit
    ROUTE_C,  // Radiology Only: Radiology → OPD → Billing → Exit
    ROUTE_D   // Pharmacy Only: Pharmacy → Billing → Exit
} RouteType;

// System constants
#define MAX_PATIENTS 100
#define MAX_DEPT_NAME 50
#define LOG_FILE "hospital_simulation.log"
#define SHM_KEY 0x1234
#define MSG_QUEUE_BASE_KEY 0x2000

// Semaphore names
#define SEM_EMERGENCY "/sem_emergency"
#define SEM_OPD "/sem_opd"
#define SEM_RADIOLOGY "/sem_radiology"
#define SEM_PHARMACY "/sem_pharmacy"
#define SEM_BILLING "/sem_billing"

// Resource counts per department
#define EMERGENCY_DOCTORS 2
#define OPD_DOCTORS 3
#define RADIOLOGY_MACHINES 1
#define PHARMACY_PHARMACISTS 2
#define BILLING_CASHIERS 1

// Treatment duration (in seconds)
#define TREATMENT_TIME_MIN 1
#define TREATMENT_TIME_MAX 3

// Round Robin time quantum (in microseconds for message processing)
#define TIME_QUANTUM 100000  // 100ms

#endif // HOSPITAL_H
