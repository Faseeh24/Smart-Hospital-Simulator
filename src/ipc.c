#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      // For O_CREAT, O_RDWR
#include <sys/mman.h>   // For shm_open, mmap, munmap
#include <unistd.h>     // For ftruncate
#include <sys/stat.h>   // For mode constants


// Helper to set up the attributes for creation
struct mq_attr default_attr = {
    .mq_flags = 0,             // Blocking queue
    .mq_maxmsg = MAX_MESSAGES, 
    .mq_msgsize = MAX_MSG_SIZE,
    .mq_curmsgs = 0            // Current messages (ignored on mq_open)
};


mqd_t ipc_mq_create_and_open() {
    // O_CREAT: Create if it doesn't exist
    // O_RDWR: Read and Write access
    // 0644: Permissions
    mqd_t mq = mq_open(MSG_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &default_attr);
    if (mq == (mqd_t)-1) {
        perror("ERROR: ipc_mq_create_and_open failed");
    }
    return mq;
}

mqd_t ipc_mq_open() {
    // O_RDWR: Read and Write access
    // NULL attributes: Only works if the queue already exists
    mqd_t mq = mq_open(MSG_QUEUE_NAME, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("ERROR: ipc_mq_open failed (Did you create it first?)");
    }
    return mq;
}

int ipc_mq_send(mqd_t mq_desc, const message_t *msg, unsigned int priority) {
    // We send the size of the whole message_t structure
    if (mq_send(mq_desc, (const char *)msg, sizeof(message_t), priority) == -1) {
        perror("ERROR: ipc_mq_send failed");
        return -1;
    }
    return 0;
}

int ipc_mq_receive(mqd_t mq_desc, message_t *msg, unsigned int *priority) {
    // The buffer size must be exactly the message size defined by the queue's attributes
    // Here we assume the received data fits into the message_t structure
    ssize_t bytes_read = mq_receive(mq_desc, (char *)msg, sizeof(message_t), priority);

    if (bytes_read == -1) {
        perror("ERROR: ipc_mq_receive failed");
        return -1;
    }
    // Optional: Check if the received size matches the expected size
    if (bytes_read != sizeof(message_t)) {
        fprintf(stderr, "WARNING: Received message size mismatch.\n");
    }

    return 0;
}

void ipc_mq_close(mqd_t mq_desc) {
    if (mq_close(mq_desc) == -1) {
        perror("ERROR: ipc_mq_close failed");
    }
}

int ipc_mq_unlink() {
    // Removes the queue name from the system, deleting the queue if all descriptors are closed
    if (mq_unlink(MSG_QUEUE_NAME) == -1) {
        perror("ERROR: ipc_mq_unlink failed (Was the queue open?)");
        return -1;
    }
    return 0;
}



// --- 1. Create and Initialize Function (The creator must run this first) ---
int ipc_shm_create_and_init() {
    // 1. Create the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("ERROR: shm_open (create) failed");
        return -1;
    }

    // 2. Set the size of the shared memory object
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ERROR: ftruncate failed");
        close(shm_fd);
        return -1;
    }

    return shm_fd;
}

// --- 2. Open Existing Function (Clients use this) ---
int ipc_shm_open() {
    // 1. Open the existing shared memory object
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("ERROR: shm_open (open) failed");
    }
    // Note: No ftruncate needed here; the creator set the size.
    return shm_fd;
}

// --- 3. Map Function ---
shared_data_t* ipc_shm_map(int shm_fd) {
    // Map the shared memory object into the process's address space
    shared_data_t *ptr = (shared_data_t*)mmap(
        0,                 // Let the kernel choose the address
        SHM_SIZE,          // Size to map
        PROT_READ | PROT_WRITE, // Protection (Read and Write)
        MAP_SHARED,        // Changes are shared with other processes
        shm_fd,            // File descriptor of the SHM object
        0                  // Offset from the start of the object
    );

    if (ptr == MAP_FAILED) {
        perror("ERROR: mmap failed");
        return NULL;
    }
    return ptr;
}

// --- 4. Unmap Function ---
int ipc_shm_unmap(shared_data_t *ptr) {
    if (munmap(ptr, SHM_SIZE) == -1) {
        perror("ERROR: munmap failed");
        return -1;
    }
    return 0;
}

// --- 5. Unlink/Delete Function (Cleanup) ---
int ipc_shm_unlink() {
    // Removes the shared memory segment name from the system
    if (shm_unlink(SHM_NAME) == -1) {
        perror("ERROR: shm_unlink failed (Was it created?)");
        return -1;
    }
    return 0;
}