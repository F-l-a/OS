#ifndef TASKS_APP_H
#define TASKS_APP_H

void vTaskReadSensor(void *params);
void vTaskControlLoop(void *params);
void vTaskLogging(void *params);
void vTaskDiagnostics(void *params);

#endif