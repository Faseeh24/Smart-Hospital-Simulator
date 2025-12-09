#include "scheduler.h"
#include <stdlib.h>

typedef struct Node {
    Patient p;
    struct Node* next;
} Node;

Node* head = NULL;

void enqueue_patient(Patient p) {
    Node* n = malloc(sizeof(Node));
    n->p = p;
    n->next = NULL;

    if(!head) { head = n; return; }

    Node* cur = head;
    while(cur->next) cur = cur->next;
    cur->next = n;
}

Patient dequeue_patient_priority() {
    Node* cur = head;
    Node* prev = NULL;

    Node* best = head;
    Node* best_prev = NULL;

    while(cur) {
        if(cur->p.severity > best->p.severity) {
            best = cur;
            best_prev = prev;
        }
        prev = cur;
        cur = cur->next;
    }

    if(best_prev == NULL)
        head = head->next;
    else
        best_prev->next = best->next;

    Patient p = best->p;
    free(best);
    return p;
}

int queue_empty() {
    return head == NULL;
}
