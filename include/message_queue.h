#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "patient.h"
#include <sys/msg.h>

// Message structure for IPC
typedef struct {
    long msg_type;  // Department type (1-5)
    int patient_id;
    RouteType route_type;
    int current_dept_index;
    time_t sent_time;
} Message;

// Message buffer with header
typedef struct {
    long mtype;
    Message data;
} MessageBuffer;

// Function declarations
int create_message_queue(int key);
int send_message_to_department(int msg_queue_id, DepartmentType dept, Patient *patient);
int receive_message_from_department(int msg_queue_id, DepartmentType dept, Message *msg, int blocking);
void destroy_message_queue(int msg_queue_id);

#endif // MESSAGE_QUEUE_H
