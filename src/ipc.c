#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "config.h"

static int msgid = -1;

struct msgbuf {
    long mtype;
    Patient patient;
};

int init_message_queue() {
    msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }
    return msgid;
}

void send_patient_msg(Patient p) {
    if (msgid == -1) {
        fprintf(stderr, "Message queue not initialized\n");
        return;
    }
    struct msgbuf msg;
    msg.mtype = 1;
    msg.patient = p;
    if (msgsnd(msgid, &msg, sizeof(Patient), 0) == -1) {
        perror("msgsnd failed");
    }
}

Patient receive_patient_msg() {
    if (msgid == -1) {
        fprintf(stderr, "Message queue not initialized\n");
        Patient empty = {0, 0, 0};
        return empty;
    }
    struct msgbuf msg;
    if (msgrcv(msgid, &msg, sizeof(Patient), 1, 0) == -1) {
        perror("msgrcv failed");
        Patient empty = {0, 0, 0};
        return empty;
    }
    return msg.patient;
}
