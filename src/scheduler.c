#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Helper: Create a New Node ---
// This function allocates memory and initializes a new node.
Node* create_node(Patient patient_data) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Failed to allocate memory for a new Node");
        return NULL;
    }
    
    // Initialize tracking variables upon creation
    // patient_data.remaining_time = patient_data.treatment_time;
    // patient_data.wait_time = 0;
    // patient_data.turnaround_time = 0;
    // patient_data.status = 0; // Set status to waiting

    new_node->p = patient_data;
    new_node->next = NULL;
    return new_node;
}


// --- 4. Ready Queue Management Implementations ---

void initialize_queue(ReadyQueue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

void enqueue_patient(ReadyQueue *q, Patient patient_data) {
    Node *new_node = create_node(patient_data);
    if (new_node == NULL) return;

    if (q->tail == NULL) {
        q->head = new_node;
        q->tail = new_node;
    } else {
        q->tail->next = new_node;
        q->tail = new_node;
    }
    q->count++;
}

Patient dequeue_patient(ReadyQueue *q) {
    if (q->head == NULL) {
        // Return a dummy patient struct if queue is empty
        Patient empty_p = {-1, 0, 0, 0, 0, 0, 0, 0};
        return empty_p; 
    }

    Node *node_to_remove = q->head;
    Patient patient_data = node_to_remove->p; // Copy the data before freeing
    
    q->head = q->head->next;
    
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(node_to_remove);
    q->count--;
    return patient_data;
}

void display_queue(const ReadyQueue *q) {
    Node *current = q->head;
    if (current == NULL) {
        printf("Ready Queue is empty.\n");
        return;
    }
    
    printf("Ready Queue (PID order): ");
    while (current != NULL) {
        printf("P%d (TT:%d)", current->p.pid, current->p.treatment_time);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
}


// --- Helper Function: Calculate and Print Averages ---
void calculate_and_print_averages(Patient completed_processes[], int num_completed) {
    int total_wt = 0;
    int total_tat = 0;
    
    printf("\n--- Results ---\n");
    printf("PID\tTT\tWT\tTAT\n");
    for (int i = 0; i < num_completed; i++) {
        total_wt += completed_processes[i].wait_time;
        total_tat += completed_processes[i].turnaround_time;
        printf("P%d\t%d\t%d\t%d\n", completed_processes[i].pid, 
               completed_processes[i].treatment_time, 
               completed_processes[i].wait_time, 
               completed_processes[i].turnaround_time);
    }
    printf("-----------------\n");
    printf("Average Waiting Time: %.2f\n", (float)total_wt / num_completed);
    printf("Average Turnaround Time: %.2f\n", (float)total_tat / num_completed);
}


// --- 5. Scheduling Algorithm Implementations ---

// A. FCFS Implementation (Non-Preemptive)
void run_fcfs(ReadyQueue *q) {
    printf("\n--- Simulating FCFS ---\n");
    
    int current_time = 0;
    int num_processes = q->count;
    // Array to store results of completed patients
    Patient *completed_list = (Patient*)calloc(num_processes, sizeof(Patient));
    int completed_count = 0;

    while (q->head != NULL) {
        Patient current_patient = dequeue_patient(q);
        
        // 1. Calculate Waiting Time
        current_patient.wait_time = current_time - current_patient.arrival_time;
        
        // 2. Execute (Status changes to in treatment, then discharged)
        current_patient.status = 1; // In treatment
        current_time += current_patient.treatment_time;
        current_patient.status = 2; // Discharged

        // 3. Calculate Turnaround Time
        current_patient.turnaround_time = current_time - current_patient.arrival_time;

        // Store completed patient for results
        completed_list[completed_count++] = current_patient;
    }
    
    calculate_and_print_averages(completed_list, num_processes);
    free(completed_list);
}


// B. SJF Implementation (Non-Preemptive)
// Note: This requires inserting processes into the queue by shortest treatment time.
// We implement a simple insertion sort for simplicity.
void enqueue_sjf(ReadyQueue *q, Patient patient_data) {
    Node *new_node = create_node(patient_data);
    if (new_node == NULL) return;

    // 1. Check if queue is empty or new node is the shortest
    if (q->head == NULL || new_node->p.treatment_time < q->head->p.treatment_time) {
        new_node->next = q->head;
        q->head = new_node;
        if (q->tail == NULL) q->tail = new_node;
        q->count++;
        return;
    }

    // 2. Find the correct insertion point
    Node *current = q->head;
    while (current->next != NULL && (current->next->p.treatment_time) <= new_node->p.treatment_time) {
        current = current->next;
    }

    // 3. Insert the node
    new_node->next = current->next;
    current->next = new_node;
    if (new_node->next == NULL) { // If inserted at the end
        q->tail = new_node;
    }
    q->count++;
}


void run_sjf_non_preemptive(ReadyQueue *initial_q) {
    printf("\n--- Simulating Non-Preemptive SJF ---\n");

    // Since the initial queue is in FCFS order, we transfer and sort them 
    // into a new, temporary queue using the SJF enqueue logic.
    ReadyQueue sjf_q;
    initialize_queue(&sjf_q);

    // Transfer and sort processes by treatment time
    while (initial_q->head != NULL) {
        enqueue_sjf(&sjf_q, dequeue_patient(initial_q));
    }
    
    printf("Queue sorted by Treatment Time (shortest first):\n");
    display_queue(&sjf_q);

    // SJF execution is now equivalent to FCFS on the sorted queue
    run_fcfs(&sjf_q); 
}


// C. Round Robin Implementation (Preemptive)
void run_round_robin(ReadyQueue *q, int quantum) {
    printf("\n--- Simulating Round Robin (Quantum=%d) ---\n", quantum);
    
    int current_time = 0;
    int num_processes = q->count;
    Patient *completed_list = (Patient*)calloc(num_processes, sizeof(Patient));
    int completed_count = 0;

    // Re-initialize remaining_time for all processes still in the queue
    Node *temp = q->head;
    while (temp != NULL) {
        temp->p.remaining_time = temp->p.treatment_time;
        temp = temp->next;
    }

    // RR loop continues as long as there are patients in the queue
    while (q->head != NULL) {
        Patient current_patient = dequeue_patient(q);
        
        // Time slice to execute (min(remaining_time, quantum))
        int exec_time = (current_patient.remaining_time > quantum) ? quantum : current_patient.remaining_time;
        
        // 1. Execute
        current_patient.remaining_time -= exec_time;
        current_time += exec_time;

        // 2. Check for Completion
        if (current_patient.remaining_time == 0) {
            // Process finished
            current_patient.status = 2; // Discharged
            current_patient.turnaround_time = current_time - current_patient.arrival_time;
            current_patient.wait_time = current_patient.turnaround_time - current_patient.treatment_time;
            
            completed_list[completed_count++] = current_patient;
        } else {
            // Process preempted, put back at the tail for its next turn
            current_patient.status = 0; // Back to waiting
            enqueue_patient(q, current_patient);
        }
    }
    
    calculate_and_print_averages(completed_list, num_processes);
    free(completed_list);
}