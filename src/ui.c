#include <stdio.h>
#include "ui.h"

void show_menu() {
    printf("\n==== SMART HOSPITAL ====\n");
    printf("1. Generate Patient\n");
    printf("2. Show Hospital State\n");
    printf("3. Exit\n");
    printf("Choice: ");
}

void display_state(HospitalState* hs) {
    printf("\n==== HOSPITAL STATE ====\n");
    printf("Beds Available: %d\n", hs->beds_available);

    for(int i=0; i<MAX_DOCTORS; i++)
        printf("Doctor %d: %s\n", i, hs->doctor_busy[i] ? "Busy" : "Free");
}
