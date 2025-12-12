#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include "hospital.h"
#include <semaphore.h>

// Department information structure
typedef struct {
    DepartmentType type;
    char name[MAX_DEPT_NAME];
    int resource_count;
    const char *sem_name;
} DepartmentInfo;

// Global department configurations
extern DepartmentInfo department_configs[NUM_DEPARTMENTS];

// Function declarations
void init_department_configs();
const char* get_department_name(DepartmentType type);
int get_department_resources(DepartmentType type);
const char* get_department_semaphore_name(DepartmentType type);
void department_process(DepartmentType dept_type);

#endif // DEPARTMENT_H
