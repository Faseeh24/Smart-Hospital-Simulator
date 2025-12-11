#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include <stddef.h> // For NULL or size_t

typedef struct {
    Node *head;
    Node *tail;
    int count;
} ReadyQueue;

// --- 4. Ready Queue Management Functions ---

// Initializes a new, empty ready queue
void initialize_queue(ReadyQueue *q);

// Creates a Node from Patient data and adds it to the ready queue (FCFS behavior)
void enqueue_patient(ReadyQueue *q, Patient patient_data);

// Removes the Patient Node from the head of the queue
Patient dequeue_patient(ReadyQueue *q);

// Prints the contents of the ready queue
void display_queue(const ReadyQueue *q);


// --- 5. Scheduling Algorithm Functions ---

// Simulates First-Come, First-Served (FCFS) scheduling
void run_fcfs(ReadyQueue *q);

// Simulates Non-Preemptive Shortest Job First (SJF) scheduling
void run_sjf_non_preemptive(ReadyQueue *q);

// Simulates Round Robin (RR) scheduling
void run_round_robin(ReadyQueue *q, int quantum);

#endif
