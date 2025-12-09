#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "types.h"

void assign_resources(Patient p, int doc_id, HospitalState* hs);
void release_resources(Patient p, int doc_id, HospitalState* hs);

#endif
