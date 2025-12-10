#include "tasks_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/* In realtà stamperete su UART o log, qui è solo esempio */

void vTaskReadSensor(void *params)
{
    printf("[HRT] ReadSensor\n");
    /* simuliamo un piccolo carico di lavoro */
    volatile int i; 
    for (i = 0; i < 10000; i++);
}

void vTaskControlLoop(void *params)
{
    printf("[HRT] ControlLoop\n");
    volatile int i;
    for (i = 0; i < 20000; i++);
}

void vTaskLogging(void *params)
{
    printf("[SRT] Logging\n");
}

void vTaskDiagnostics(void *params)
{
    printf("[SRT] Diagnostics\n");
}