#include <stdio.h>
#include "ipc.h"
#include "hospital.h"
#include "patient.h"
#include "scheduler.h"
#include "doctor.h"
#include "ui.h"
#include "utils.h"

int main() {
    create_message_queue();
    HospitalState* hs = attach_shared_memory();
    init_hospital(hs);
    start_doctors(hs);

    while(1) {
        show_menu();
        int ch; scanf("%d", &ch);

        if(ch == 1) {
            spawn_patient();
            Patient p = receive_patient();
            enqueue_patient(p);
        }
        else if(ch == 2) {
            display_state(hs);
        }
        else if(ch == 3) {
            break;
        }
    }

    detach_shared_memory(hs);
    return 0;
}
