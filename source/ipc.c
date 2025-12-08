#include "../headers/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int create_message_queue(key_t key) {
    int msqid;
    if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }
    return msqid;
}

int send_message(int msqid, struct msgbuf *msg) {
    if (msgsnd(msqid, msg, sizeof(msg->mtext), 0) == -1) {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int receive_message(int msqid, struct msgbuf *msg, long msgtyp) {
    if (msgrcv(msqid, msg, sizeof(msg->mtext), msgtyp, 0) == -1) {
        perror("msgrcv");
        return -1;
    }
    return 0;
}

int ipc_delete_msgqueue(int msqid) {
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return -1;
    }
    return 0;
}

int create_shared_memory(key_t key, size_t size) {
    int shmid;
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

struct HospitalState *attach_shared_memory(int shmid) {
    struct HospitalState *shm_ptr;
    if ((shm_ptr = (struct HospitalState *)shmat(shmid, NULL, 0)) == (struct HospitalState *)-1) {
        perror("shmat");
        return NULL;
    }
    return shm_ptr;
}

int detach_shared_memory(struct HospitalState *shm_ptr) {
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        return -1;
    }
    return 0;
}

int destroy_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        return -1;
    }
    return 0;
}
