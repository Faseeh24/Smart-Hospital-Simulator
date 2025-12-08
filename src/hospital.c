#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "config.h"

static int shmid = -1;

HospitalState* init_hospital_state() {
    shmid = shmget(SHM_KEY, sizeof(HospitalState), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    HospitalState* hs = (HospitalState*) shmat(shmid, NULL, 0);
    if (hs == (HospitalState*) -1) {
        perror("shmat failed");
        exit(1);
    }
    // Initialize the hospital state
    hs->total_beds = 10;
    hs->available_beds = 10;
    hs->total_doctors = MAX_DOCTORS;
    memset(hs->doctor_busy, 0, sizeof(hs->doctor_busy));
    hs->total_equipments = MAX_EQUIPMENT;
    hs->available_equipments = MAX_EQUIPMENT;
    hs->treated_patients = 0;
    return hs;
}

void save_hospital_state(HospitalState* hs) {
    // Detach the shared memory segment
    if (shmdt(hs) == -1) {
        perror("shmdt failed");
    }
}

void close_shared_memory(HospitalState* hs) {
    // Remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }
}
