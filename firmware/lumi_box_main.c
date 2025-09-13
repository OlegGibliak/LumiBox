#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "gui.h"

int main(void) {
    
    core_init();
    gui_init();

    HAL_Delay(1000);

    gui_process_task(NULL);
}
