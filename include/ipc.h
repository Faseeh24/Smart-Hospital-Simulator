#ifndef IPC_H
#define IPC_H

#include "types.h"

#define MSG_KEY 1234
#define SHM_KEY 5678

typedef struct {
    long mtype;
    Patient p;
} Message;

int create_message_queue();
int send_patient(Patient p);
Patient receive_patient();

HospitalState* attach_shared_memory();
void detach_shared_memory(HospitalState* hs);

#endif
