#include "scheduler.h"
#include "logger.h"
#include "department.h"
#include "message_queue.h"
#include <stdlib.h>
#include <unistd.h>

// Enqueue patient to ready queue (FCFS)
void enqueue_patient(SchedulerNode **queue, Patient *patient) {
    SchedulerNode *new_node = (SchedulerNode*)malloc(sizeof(SchedulerNode));
    if (!new_node) {
        log_message(LOG_ERROR, "Failed to allocate scheduler node");
        return;
    }
    
    new_node->patient = patient;
    new_node->enqueue_time = time(NULL);
    new_node->next = NULL;
    
    if (*queue == NULL) {
        *queue = new_node;
    } else {
        SchedulerNode *current = *queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    
    log_message(LOG_DEBUG, "Patient %d enqueued to ready queue", patient->id);
}

// Dequeue patient from ready queue (FCFS)
Patient* dequeue_patient(SchedulerNode **queue) {
    if (*queue == NULL) {
        return NULL;
    }
    
    SchedulerNode *node = *queue;
    Patient *patient = node->patient;
    *queue = node->next;
    
    time_t wait_time = time(NULL) - node->enqueue_time;
    patient->total_waiting_time += wait_time;
    
    free(node);
    
    log_message(LOG_DEBUG, "Patient %d dequeued from ready queue (waited %lds)", 
                patient->id, wait_time);
    return patient;
}

// Check if queue is empty
int is_queue_empty(SchedulerNode *queue) {
    return (queue == NULL);
}

// FCFS Scheduler - schedules patients to departments
void fcfs_scheduler(SchedulerNode **ready_queue, int msg_queue_id) {
    if (is_queue_empty(*ready_queue)) {
        return;
    }
    
    Patient *patient = dequeue_patient(ready_queue);
    if (!patient) return;
    
    DepartmentType next_dept = get_next_department(patient);
    
    if (next_dept == (DepartmentType)-1) {
        // Patient route complete
        patient->completed = 1;
        patient->discharge_time = time(NULL);
        log_message(LOG_INFO, "Patient %d completed all treatments and discharged", patient->id);
        return;
    }
    
    // Send patient to next department
    send_message_to_department(msg_queue_id, next_dept, patient);
    patient->current_dept_index++;
}

// Round Robin Message Scheduler
void round_robin_message_scheduler(int msg_queue_id, Patient **all_patients, int num_patients) {
    log_message(LOG_INFO, "Round Robin message scheduler started");
    
    int department_turn = 0;  // Round robin among departments
    int messages_processed = 0;
    int max_iterations = num_patients * 10;  // Safety limit
    
    while (messages_processed < max_iterations) {
        // Try to receive completion message from any department
        MessageBuffer msg_buf;
        ssize_t result = msgrcv(msg_queue_id, &msg_buf, sizeof(Message), 
                               NUM_DEPARTMENTS + 1, IPC_NOWAIT);
        
        if (result != -1) {
            messages_processed++;
            
            // Find the patient and update
            for (int i = 0; i < num_patients; i++) {
                if (all_patients[i]->id == msg_buf.data.patient_id) {
                    Patient *patient = all_patients[i];
                    
                    DepartmentType next_dept = get_next_department(patient);
                    
                    if (next_dept == (DepartmentType)-1) {
                        // Patient completed
                        patient->completed = 1;
                        patient->discharge_time = time(NULL);
                        log_message(LOG_INFO, "Patient %d completed all treatments", patient->id);
                    } else {
                        // Send to next department
                        usleep(TIME_QUANTUM);  // Time quantum delay (Round Robin)
                        send_message_to_department(msg_queue_id, next_dept, patient);
                        patient->current_dept_index++;
                    }
                    break;
                }
            }
        } else {
            // No message available, small delay
            usleep(50000);  // 50ms
        }
        
        // Check if all patients completed
        int all_done = 1;
        for (int i = 0; i < num_patients; i++) {
            if (!all_patients[i]->completed) {
                all_done = 0;
                break;
            }
        }
        
        if (all_done) {
            log_message(LOG_INFO, "All patients completed treatment");
            break;
        }
        
        // Round robin turn
        department_turn = (department_turn + 1) % NUM_DEPARTMENTS;
    }
    
    log_message(LOG_INFO, "Message scheduler finished");
}
