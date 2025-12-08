#include "../headers/hospital.h"

void init_hospital_state(struct HospitalState *state, int total_beds, int total_doctors, int total_equipments) {
    state->total_beds = total_beds;
    state->total_doctors = total_doctors;
    state->total_equipments = total_equipments;
    state->num_patients = 0;
    state->num_doctors = 0;
    state->num_nurses = 0;
}

void detach_destroy_shared_state(struct HospitalState *state) {
    detach_shared_memory(state);
    // Note: destroy_shared_memory would need the shmid, but since we don't have it here,
    // this function only detaches. Destroy should be called separately if needed.
}
