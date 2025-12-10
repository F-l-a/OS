/**
 * @file trace.h
 * @brief Public interface for the scheduler tracing system.
 *
 * This file defines the API for logging scheduler and task events with
 * tick-level precision for debugging and validation purposes.
 */

#ifndef TRACE_H
#define TRACE_H

#include "FreeRTOS.h"

/**
 * @brief Enumeration of events that can be logged by the trace system.
 */
typedef enum {
    TRACE_EVENT_MAJOR_FRAME_START,
    TRACE_EVENT_TASK_SPAWN,
    TRACE_EVENT_TASK_COMPLETE,
    TRACE_EVENT_DEADLINE_MISS,
    TRACE_EVENT_TASK_CREATE_FAILED,
    TRACE_EVENT_IDLE_START,
    TRACE_EVENT_IDLE_END,
} TraceEvent_t;

/**
 * @brief Initializes the tracing system.
 *
 * Must be called before any other trace function.
 */
void vTraceInit(void);

/**
 * @brief Logs a scheduler event.
 *
 * This function is thread-safe and can be called from any task.
 *
 * @param xEvent The type of event to log.
 * @param pcTaskName The name of the task associated with the event.
 * @param xTick The tick count at which the event occurred.
 */
void vTraceLog(TraceEvent_t xEvent, const char *pcTaskName, TickType_t xTick);

#endif // TRACE_H
