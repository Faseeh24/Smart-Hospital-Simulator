#ifndef TYPES_H
#define TYPES_H

typedef struct
{
    int id;
    int priority;
    int arrival_time;
    int treatment_time;
    int status; // 0: waiting, 1: in treatment, 2: discharged
} Patient;

typedef struct {
    Patient p;
    struct Node* next;
} Node;


#endif
