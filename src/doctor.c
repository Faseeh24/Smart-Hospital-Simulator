#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "doctor.h"
#include "scheduler.h"
#include "resource_manager.h"
#include "types.h"

HospitalState* global_hs;

void* doctor_thread(void* arg) {
    int doc_id = *(int*) arg;

    while(1) {
        if(queue_empty()) continue;

        Patient p = dequeue_patient_priority();
        assign_resources(p, doc_id, global_hs);
        
        sleep(p.required_time);

        release_resources(p, doc_id, global_hs);
    }
}

void start_doctors(HospitalState* hs) {
    global_hs = hs;

    pthread_t tid[MAX_DOCTORS];
    int ids[MAX_DOCTORS];

    for(int i=0; i<MAX_DOCTORS; i++) {
        ids[i] = i;
        pthread_create(&tid[i], NULL, doctor_thread, &ids[i]);
    }
}
