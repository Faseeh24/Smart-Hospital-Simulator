#ifndef IPC_H
#define IPC_H

#include <mqueue.h>
#include <stddef.h>

#define MSG_QUEUE_NAME "/hospital_mq"
#define MAX_MSG_SIZE 256
#define MAX_MESSAGES 10

#define SHM_NAME "/hospital_shm"
#define DATA_SIZE 4096
#define MAX_KEY_LEN    32     // Max length of a key string (e.g., "PatientID")
#define MAX_VALUE_LEN  128    // Max length of a value string (e.g., "Completed/Awaiting")
#define MAX_PAIRS      10

typedef struct {
    int pid;          // Process ID of the sender (e.g., Doctor, Patient, etc.)
    int type;         // Message type (e.g., 1=REQUEST, 2=RESULT, 3=TERMINATE)
    char data[MAX_MSG_SIZE]; // Actual message content
} message_t;

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} kv_pair_t;

typedef struct {
    int current_processes;  // E.g., counter of active processes
    int next_patient_id;    // E.g., ID generator state
    kv_pair_t state_data[MAX_PAIRS]; 
    int current_pair_count;
    int update_flag; // if 1 means A process is currently modifying the array and/or the current_pair_count. DO NOT READ OR WRITE.
} shared_data_t;
// The total size of the shared memory segment
#define SHM_SIZE sizeof(shared_data_t)


// CREATE and OPEN the message queue (often done by the server/controller)
mqd_t ipc_mq_create_and_open();

// OPEN an existing message queue (often done by clients/workers)
mqd_t ipc_mq_open();

// SEND a message
int ipc_mq_send(mqd_t mq_desc, const message_t *msg, unsigned int priority);

// RECEIVE a message
int ipc_mq_receive(mqd_t mq_desc, message_t *msg, unsigned int *priority);

// CLOSE the message queue descriptor
void ipc_mq_close(mqd_t mq_desc);

// UNLINK/DELETE the message queue (clean-up)
int ipc_mq_unlink();


// Function to CREATE and INITIALIZE the shared memory segment (Server/Controller)
// Returns the file descriptor (fd) of the segment.
int ipc_shm_create_and_init();

// Function to OPEN an existing shared memory segment (Client/Worker)
// Returns the file descriptor (fd) of the segment.
int ipc_shm_open();

// Function to MAP the shared memory segment into the process's address space
// Returns a pointer to the shared_data_t structure.
shared_data_t* ipc_shm_map(int shm_fd);

// Function to UNMAP the shared memory segment from the process's address space
int ipc_shm_unmap(shared_data_t *ptr);

// Function to UNLINK/DELETE the shared memory segment from the system (Cleanup)
int ipc_shm_unlink();

#endif
