#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "ipc.h"
#include "types.h"
#include "patient.h"

void spawn_patient() {
    int pid = fork();

    if(pid == 0) {
        srand(time(NULL));

        Patient p;
        p.patient_id = rand() % 1000;
        p.severity = rand() % 3;
        p.required_time = (p.severity + 1) * 2;

        send_patient(p);
        exit(0);
    }
    else {
        printf("Spawned patient process: %d\n", pid);
    }
}
