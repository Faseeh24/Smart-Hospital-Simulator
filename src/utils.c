#include <stdio.h>
#include "utils.h"

void clear_screen() {
    printf("\033[2J\033[1;1H");
}
