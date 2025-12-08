typedef struct {
    int patient_id;
    int severity; // 0 = normal, 1 = urgent, 2 = critical
    int required_time;
} Patient;

typedef struct {
    int total_beds;
    int available_beds;

    int total_doctors;
    int doctor_busy[10]; // 1 = busy, 0 = free

    int total_equipments;
    int available_equipments;

    int treated_patients;
} HospitalState;

typedef struct {
    Patient patient;
    struct Node *next;
} Node;
