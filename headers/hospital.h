#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>

#define SHM_KEY IPC_PRIVATE

struct HospitalState *attach_shared_state() {
    int shmid = create_shared_memory(SHM_KEY, sizeof(struct HospitalState));
    struct HospitalState *state = attach_shared_memory(shmid);
    if (state == NULL) {
        return NULL;
    }
    return state;
}

void init_hospital_state(struct HospitalState *state, int total_beds, int total_doctors, int total_equipments);
void detach_destroy_shared_state(struct HospitalState *state);

#endif