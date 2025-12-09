#include "resource_manager.h"
#include <stdio.h>

void assign_resources(Patient p, int doc, HospitalState* hs) {
    for(int i=0; i<MAX_BEDS; i++) {
        if(!hs->beds[i].occupied) {
            hs->beds[i].occupied = 1;
            hs->beds[i].patient_id = p.patient_id;
            hs->beds_available--;
            break;
        }
    }
    hs->doctor_busy[doc] = 1;

    printf("Patient %d assigned to doctor %d\n", 
           p.patient_id, doc);
}

void release_resources(Patient p, int doc, HospitalState* hs) {
    for(int i=0; i<MAX_BEDS; i++) {
        if(hs->beds[i].patient_id == p.patient_id) {
            hs->beds[i].occupied = 0;
            hs->beds_available++;
        }
    }
    hs->doctor_busy[doc] = 0;

    printf("Patient %d released by doctor %d\n", 
           p.patient_id, doc);
}
