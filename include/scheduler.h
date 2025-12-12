#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "patient.h"
#include "message_queue.h"

// Scheduler queue node
typedef struct SchedulerNode {
    Patient *patient;
    time_t enqueue_time;
    struct SchedulerNode *next;
} SchedulerNode;

// Function declarations - FCFS Queue operations
void enqueue_patient(SchedulerNode **queue, Patient *patient);
Patient* dequeue_patient(SchedulerNode **queue);
int is_queue_empty(SchedulerNode *queue);

// Scheduler functions
void fcfs_scheduler(SchedulerNode **ready_queue, int msg_queue_id);
void round_robin_message_scheduler(int msg_queue_id, Patient **all_patients, int num_patients);

#endif // SCHEDULER_H
