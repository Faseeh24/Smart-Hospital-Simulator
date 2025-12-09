#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

void enqueue_patient(Patient p);
Patient dequeue_patient_priority();
int queue_empty();

#endif
