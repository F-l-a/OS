/**
 * @file timeline_scheduler.c
 * @brief Implementation of the Timeline-based Scheduler for FreeRTOS.
 *
 * This scheduler replaces the default priority-based scheduling with a deterministic,
 * time-triggered approach based on a major/sub-frame architecture.
 */

#include "timeline_scheduler.h"
#include "trace.h"
#include <string.h> // For memset

// --- Private Data Structures ---

/**
 * @brief Internal state of a managed task.
 */
typedef struct {
    const TimelineTaskConfig_t *pxConfig; /**< Pointer to the public task configuration. */
    TaskHandle_t xHandle;                 /**< Handle of the FreeRTOS task. */
    BaseType_t xIsActive;                 /**< Flag to indicate if the task is currently running. */
} ManagedTask_t;

// --- Private State ---

static TimelineConfig_t xSchedulerConfig;
static ManagedTask_t xManagedTasks[MAX_TASKS];
static UBaseType_t uxManagedTasksCount = 0;
static TaskHandle_t xSchedulerTaskHandle = NULL;

// --- Private Functions ---

/**
 * @brief The main scheduler task.
 *
 * This high-priority task manages the entire timeline, including the major frame
 * cycle and the spawning/termination of HRT and SRT tasks.
 *
 * @param pvParameters Unused.
 */
static void prvSchedulerTask(void *pvParameters) {
    TickType_t xMajorFrameStartTick;

    // This task starts automatically after vTaskStartScheduler() is called.
    vTimelineSchedulerStart();

    for (;;) {
        xMajorFrameStartTick = xTaskGetTickCount();
        vTraceLog(TRACE_EVENT_MAJOR_FRAME_START, "Scheduler", xMajorFrameStartTick);

        // --- HRT Task Scheduling Phase ---
        for (UBaseType_t i = 0; i < uxManagedTasksCount; i++) {
            if (xManagedTasks[i].pxConfig->xTaskType == TASK_TYPE_HARD_RT) {
                TickType_t xCurrentTick = xTaskGetTickCount();
                TickType_t xStartTime = xMajorFrameStartTick + xManagedTasks[i].pxConfig->ulStartTimeTicks;
                
                // Wait until the task's start time
                if (xCurrentTick < xStartTime) {
                    vTaskDelay(xStartTime - xCurrentTick);
                }

                // Spawn the HRT task
                // A new task is created for each execution, as per the project requirements (start-to-end execution)
                xTaskCreate(xManagedTasks[i].pxConfig->pvTaskCode,
                            xManagedTasks[i].pxConfig->pcName,
                            configMINIMAL_STACK_SIZE,
                            NULL,
                            tskIDLE_PRIORITY + 2, // Higher priority than scheduler to run immediately
                            &xManagedTasks[i].xHandle);
                
                if (xManagedTasks[i].xHandle == NULL) {
                    vTraceLog(TRACE_EVENT_TASK_CREATE_FAILED, xManagedTasks[i].pxConfig->pcName, xTaskGetTickCount());
                    continue; // Skip to next task if creation failed
                }

                vTraceLog(TRACE_EVENT_TASK_SPAWN, xManagedTasks[i].pxConfig->pcName, xTaskGetTickCount());
                xManagedTasks[i].xIsActive = pdTRUE;

                // Monitor for deadline
                TickType_t xDeadline = xMajorFrameStartTick + xManagedTasks[i].pxConfig->ulEndTimeTicks;
                
                // Loop to check for task completion or deadline miss
                while (xManagedTasks[i].xIsActive) {
                    // Check if the task has completed by deleting itself
                    // eTaskGetState is not perfectly reliable for this, a better approach would be a notification
                    if (eTaskGetState(xManagedTasks[i].xHandle) == eDeleted) {
                        xManagedTasks[i].xIsActive = pdFALSE;
                        vTraceLog(TRACE_EVENT_TASK_COMPLETE, xManagedTasks[i].pxConfig->pcName, xTaskGetTickCount());
                        break; // Exit while loop
                    }

                    // Check if the deadline is reached
                    if (xTaskGetTickCount() >= xDeadline) {
                        vTaskDelete(xManagedTasks[i].xHandle);
                        xManagedTasks[i].xIsActive = pdFALSE;
                        vTraceLog(TRACE_EVENT_DEADLINE_MISS, xManagedTasks[i].pxConfig->pcName, xTaskGetTickCount());
                        break; // Exit while loop
                    }
                    
                    // Small delay to prevent this loop from consuming 100% CPU
                    vTaskDelay(1);
                }
            }
        }

        // --- SRT Task Scheduling Phase ---
        // This phase would run SRT tasks in the remaining time.
        // For simplicity in this first step, this is left as a placeholder.
        vTraceLog(TRACE_EVENT_IDLE_START, "Scheduler", xTaskGetTickCount());
        
        // Wait for the end of the major frame
        TickType_t xMajorFrameEndTick = xMajorFrameStartTick + MAJOR_FRAME_DURATION_TICKS;
        TickType_t xCurrentTick = xTaskGetTickCount();
        if (xCurrentTick < xMajorFrameEndTick) {
            vTaskDelay(xMajorFrameEndTick - xCurrentTick);
        }
        vTraceLog(TRACE_EVENT_IDLE_END, "Scheduler", xTaskGetTickCount());
    }
}

// --- Public API Implementation ---

BaseType_t xTimelineSchedulerInit(const TimelineConfig_t *pxTimelineConfig) {
    if (pxTimelineConfig == NULL || pxTimelineConfig->pxTasks == NULL || pxTimelineConfig->uxNumTasks > MAX_TASKS) {
        return pdFAIL;
    }

    xSchedulerConfig = *pxTimelineConfig;
    uxManagedTasksCount = xSchedulerConfig.uxNumTasks;

    // Initialize managed tasks array
    memset(xManagedTasks, 0, sizeof(xManagedTasks));
    for (UBaseType_t i = 0; i < uxManagedTasksCount; i++) {
        xManagedTasks[i].pxConfig = &xSchedulerConfig.pxTasks[i];
        xManagedTasks[i].xHandle = NULL;
        xManagedTasks[i].xIsActive = pdFALSE;
    }
    
    vTraceInit();

    // Create the main scheduler task here, so it's ready to run when the scheduler starts
    xTaskCreate(prvSchedulerTask,
                "Scheduler",
                configMINIMAL_STACK_SIZE * 2,
                NULL,
                tskIDLE_PRIORITY + 1, // High priority, but lower than HRT tasks it spawns
                &xSchedulerTaskHandle);

    return pdPASS;
}

void vTimelineSchedulerStart(void) {
    // This function is now primarily for conceptual separation.
    // The scheduler task is created during init and will run automatically.
    // We could add a synchronization mechanism here if we needed to delay
    // the start of the major frame loop, but for now it's not required.
}
