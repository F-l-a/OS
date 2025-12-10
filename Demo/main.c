#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "timeline_scheduler.h"
#include "trace.h"

// --- Task Implementations ---

/**
 * @brief A simple Hard Real-Time task that completes on time.
 */
void vTask_HRT1(void *pvParameters) {
    (void)pvParameters;
    uart_puts("HRT1: Running\r\n");
    vTaskDelay(pdMS_TO_TICKS(20)); // Simulate work
    uart_puts("HRT1: Completed\r\n");
    // Task must delete itself upon completion
    vTaskDelete(NULL);
}

/**
 * @brief A Hard Real-Time task designed to miss its deadline.
 */
void vTask_HRT2_DeadlineMiss(void *pvParameters) {
    (void)pvParameters;
    uart_puts("HRT2: Running (will miss deadline)\r\n");
    // This delay will exceed the configured deadline of 30ms
    vTaskDelay(pdMS_TO_TICKS(50)); 
    uart_puts("HRT2: Should have been terminated\r\n");
    vTaskDelete(NULL);
}

/**
 * @brief A Soft Real-Time task.
 * Note: The current scheduler implementation does not yet execute SRT tasks.
 */
void vTask_SRT1(void *pvParameters) {
    (void)pvParameters;
    uart_puts("SRT1: Running\r\n");
    vTaskDelete(NULL);
}


// --- Scheduler Configuration ---

const TimelineTaskConfig_t xMyTasks[] = {
    { vTask_HRT1, "HRT1", TASK_TYPE_HARD_RT, pdMS_TO_TICKS(10), pdMS_TO_TICKS(40), 1 },
    { vTask_HRT2_DeadlineMiss, "HRT2", TASK_TYPE_HARD_RT, pdMS_TO_TICKS(50), pdMS_TO_TICKS(80), 2 },
    // { vTask_SRT1, "SRT1", TASK_TYPE_SOFT_RT, 0, 0, 0 }, // SRT task, not yet scheduled
};

const TimelineConfig_t xMyTimeline = {
    .pxTasks = xMyTasks,
    .uxNumTasks = sizeof(xMyTasks) / sizeof(TimelineTaskConfig_t)
};


int main(int argc, char **argv){
	(void) argc;
	(void) argv;

    UART_init();
    uart_puts("--- Timeline Scheduler Demo ---\r\n");

    // Initialize the timeline scheduler with our configuration
	if (xTimelineSchedulerInit(&xMyTimeline) != pdPASS) {
        uart_puts("ERROR: Failed to initialize timeline scheduler.\r\n");
        for(;;);
    }

	// Start the FreeRTOS scheduler
	vTaskStartScheduler();

    // The timeline scheduler's control task is started by vTaskStartScheduler.
    // We now need to start the timeline itself.
    // NOTE: In a real application, vTimelineSchedulerStart might be called
    // by a startup task after the scheduler is running. For simplicity,
    // we assume it's implicitly handled or would be called from a hook.
    // The current design starts it from within vTaskStartScheduler's created tasks.
    
    // If everything is okay, the program should never reach here.
    for( ; ; );
}

