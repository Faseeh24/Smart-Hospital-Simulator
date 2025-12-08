#ifndef IPC_H
#define IPC_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define MSG_SIZE 100

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

struct HospitalState {
    int total_beds;
    int total_doctors;
    int total_equipments;
    int num_patients;
    int num_doctors;
    int num_nurses;
    // Add more fields as needed
};

int create_message_queue(key_t key);
int send_message(int msqid, struct msgbuf *msg);
int receive_message(int msqid, struct msgbuf *msg, long msgtyp);
int ipc_delete_msgqueue(int msqid);

int create_shared_memory(key_t key, size_t size);
struct HospitalState *attach_shared_memory(int shmid);
int detach_shared_memory(struct HospitalState *shm_ptr);
int destroy_shared_memory(int shmid);

#endif
