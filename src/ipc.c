#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "ipc.h"

int qid;

int create_message_queue() {
    qid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if(qid < 0) { perror("msgget"); exit(1); }
    return qid;
}

int send_patient(Patient p) {
    Message m;
    m.mtype = 1;
    m.p = p;
    return msgsnd(qid, &m, sizeof(Patient), 0);
}

Patient receive_patient() {
    Message m;
    msgrcv(qid, &m, sizeof(Patient), 1, 0);
    return m.p;
}

HospitalState* attach_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(HospitalState), 0666 | IPC_CREAT);
    if(shmid < 0) { perror("shmget"); exit(1); }
    return (HospitalState*) shmat(shmid, NULL, 0);
}

void detach_shared_memory(HospitalState* hs) {
    shmdt(hs);
}
