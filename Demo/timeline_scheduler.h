/**
 * @file timeline_scheduler.h
 * @brief Public interface for the Timeline-based Scheduler for FreeRTOS.
 *
 * This file defines the data structures and APIs required to configure and run
 * the deterministic, timeline-based scheduler.
 */

#ifndef TIMELINE_SCHEDULER_H
#define TIMELINE_SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"

// --- Public Configuration ---

/**
 * @brief Defines the duration of the major frame in ticks.
 * All task timelines are relative to the start of this frame.
 */
#define MAJOR_FRAME_DURATION_TICKS pdMS_TO_TICKS(100)

/**
 * @brief Maximum number of tasks the scheduler can manage.
 */
#define MAX_TASKS 16

/**
 * @brief Defines the type of a task.
 */
typedef enum {
    TASK_TYPE_HARD_RT, /**< Hard Real-Time: runs at a fixed time, non-preemptible by other tasks. */
    TASK_TYPE_SOFT_RT  /**< Soft Real-Time: runs in idle time, preemptible by HRT tasks. */
} TaskType_t;

/**
 * @brief Configuration structure for a single task in the timeline.
 */
typedef struct {
    TaskFunction_t pvTaskCode;      /**< Pointer to the task's function. */
    const char *pcName;             /**< A descriptive name for the task. */
    TaskType_t xTaskType;           /**< The type of the task (HARD_RT or SOFT_RT). */
    uint32_t ulStartTimeTicks;      /**< Start time in ticks from the beginning of the major frame (for HRT tasks). */
    uint32_t ulEndTimeTicks;        /**< Deadline in ticks from the beginning of the major frame (for HRT tasks). */
    uint32_t ulSubframeId;          /**< ID of the sub-frame this task belongs to (for HRT tasks, informational). */
} TimelineTaskConfig_t;

/**
 * @brief Main configuration structure for the timeline scheduler.
 *
 * This structure holds the array of task configurations and the total number of tasks.
 */
typedef struct {
    const TimelineTaskConfig_t *pxTasks; /**< Array of task configurations. */
    UBaseType_t uxNumTasks;              /**< Number of tasks in the array. */
} TimelineConfig_t;


// --- Public API ---

/**
 * @brief Initializes and starts the timeline-based scheduler.
 *
 * This function configures the scheduler based on the provided timeline definition,
 * creates the necessary internal data structures, and starts the scheduler's control task.
 *
 * @param pxTimelineConfig Pointer to the main timeline configuration structure.
 * @return pdPASS if initialization was successful, pdFAIL otherwise.
 */
BaseType_t xTimelineSchedulerInit(const TimelineConfig_t *pxTimelineConfig);

/**
 * @brief Starts the execution of the major frame loop.
 *
 * This function should be called after the FreeRTOS scheduler has been started.
 * It will begin the cyclical execution of the defined timeline.
 */
void vTimelineSchedulerStart(void);

#endif // TIMELINE_SCHEDULER_H
