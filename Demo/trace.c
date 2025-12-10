/**
 * @file trace.c
 * @brief Implementation of the scheduler tracing system.
 *
 * This module provides a simple, thread-safe logging mechanism to record
 * scheduler events. It uses a mutex to protect access to the underlying
 * communication peripheral (e.g., UART).
 */

#include "trace.h"
#include "uart.h" // Assuming a simple UART driver for output
#include "semphr.h"
#include <stdio.h>

// --- Private State ---

static SemaphoreHandle_t xTraceMutex = NULL;

// --- Public API Implementation ---

void vTraceInit(void) {
    xTraceMutex = xSemaphoreCreateMutex();
    if (xTraceMutex != NULL) {
        // Initialization successful
    }
}

void vTraceLog(TraceEvent_t xEvent, const char *pcTaskName, TickType_t xTick) {
    if (xSemaphoreTake(xTraceMutex, portMAX_DELAY) == pdPASS) {
        char cBuffer[100];
        const char *pcEventStr = "UNKNOWN";

        switch (xEvent) {
            case TRACE_EVENT_MAJOR_FRAME_START: pcEventStr = "MAJOR_FRAME_START"; break;
            case TRACE_EVENT_TASK_SPAWN:        pcEventStr = "SPAWN"; break;
            case TRACE_EVENT_TASK_COMPLETE:     pcEventStr = "COMPLETE"; break;
            case TRACE_EVENT_DEADLINE_MISS:     pcEventStr = "DEADLINE_MISS"; break;
            case TRACE_EVENT_TASK_CREATE_FAILED:pcEventStr = "CREATE_FAILED"; break;
            case TRACE_EVENT_IDLE_START:        pcEventStr = "IDLE_START"; break;
            case TRACE_EVENT_IDLE_END:          pcEventStr = "IDLE_END"; break;
        }

        sprintf(cBuffer, "[%5lu] %-10s: %s\r\n", (unsigned long)xTick, pcTaskName, pcEventStr);
        
        // Assuming uart_puts is a function from your uart driver
        uart_puts(cBuffer);

        xSemaphoreGive(xTraceMutex);
    }
}
