#include "hospital.h"
#include <stdio.h>

void init_hospital(HospitalState* hs) {
    hs->beds_available = MAX_BEDS;
    
    for(int i=0; i<MAX_DOCTORS; i++)
        hs->doctor_busy[i] = 0;

    for(int i=0; i<MAX_BEDS; i++) {
        hs->beds[i].occupied = 0;
        hs->beds[i].patient_id = -1;
    }
}

void print_hospital(HospitalState* hs) {
    printf("\n--- HOSPITAL STATE ---\n");
    printf("Beds available: %d\n", hs->beds_available);

    printf("Doctors:\n");
    for(int i=0; i<MAX_DOCTORS; i++)
        printf("Doctor %d: %s\n", i, hs->doctor_busy[i] ? "Busy" : "Free");

    printf("Beds:\n");
    for(int i=0; i<MAX_BEDS; i++)
        printf("Bed %d: %s\n", i, hs->beds[i].occupied ? "Occupied" : "Empty");
}
