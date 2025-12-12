#include "message_queue.h"
#include "logger.h"
#include "department.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

// Create message queue
int create_message_queue(int key) {
    int msg_queue_id = msgget(key, IPC_CREAT | 0666);
    if (msg_queue_id == -1) {
        log_message(LOG_ERROR, "Failed to create message queue: %s", strerror(errno));
        return -1;
    }
    
    log_message(LOG_INFO, "Message queue created with ID: %d", msg_queue_id);
    return msg_queue_id;
}

// Send message to specific department
int send_message_to_department(int msg_queue_id, DepartmentType dept, Patient *patient) {
    if (!patient) return -1;
    
    MessageBuffer msg_buf;
    msg_buf.mtype = dept + 1;  // Message type 1-5 for departments
    msg_buf.data.patient_id = patient->id;
    msg_buf.data.route_type = patient->route_type;
    msg_buf.data.current_dept_index = patient->current_dept_index;
    msg_buf.data.sent_time = time(NULL);
    
    if (msgsnd(msg_queue_id, &msg_buf, sizeof(Message), 0) == -1) {
        log_message(LOG_ERROR, "Failed to send message for Patient %d to %s: %s",
                    patient->id, get_department_name(dept), strerror(errno));
        return -1;
    }
    
    log_message(LOG_INFO, "Sent Patient %d to %s department", 
                patient->id, get_department_name(dept));
    return 0;
}

// Receive message from department
int receive_message_from_department(int msg_queue_id, DepartmentType dept, Message *msg, int blocking) {
    if (!msg) return -1;
    
    MessageBuffer msg_buf;
    int flags = blocking ? 0 : IPC_NOWAIT;
    long msg_type = dept + 1;  // Receive messages for this department
    
    ssize_t result = msgrcv(msg_queue_id, &msg_buf, sizeof(Message), msg_type, flags);
    
    if (result == -1) {
        if (errno != ENOMSG) {  // ENOMSG is expected for non-blocking when no message
            log_message(LOG_ERROR, "Failed to receive message from %s: %s",
                        get_department_name(dept), strerror(errno));
        }
        return -1;
    }
    
    *msg = msg_buf.data;
    return 0;
}

// Destroy message queue
void destroy_message_queue(int msg_queue_id) {
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        log_message(LOG_ERROR, "Failed to destroy message queue: %s", strerror(errno));
    } else {
        log_message(LOG_INFO, "Message queue destroyed");
    }
}
