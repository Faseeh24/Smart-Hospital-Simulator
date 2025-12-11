#ifndef TYPES_H
#define TYPES_H

typedef struct
{
    int pid;
    int priority;
    int arrival_time;
    int treatment_time; // like burst time
    int status; // 0: waiting, 1: in treatment, 2: discharged

    // Variables added for tracking/scheduling logic
    int remaining_time; // for SJF and RR
    int wait_time;
    int turnaround_time;
} Patient;

struct Node; // Forward declaration
typedef struct Node Node;

struct Node {
    Patient p;
    Node* next;
};


#endif
