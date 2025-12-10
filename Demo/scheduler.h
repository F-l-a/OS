#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* Tipo di task nella timeline */
typedef enum {
    TASK_HARD_RT,
    TASK_SOFT_RT
} TaskType_t;

/* Configurazione di un singolo task nella timeline */
typedef struct {
    const char*    task_name;
    TaskFunction_t function;      // puntatore alla funzione C
    TaskType_t     type;          // HARD o SOFT
    uint32_t       ulStart_time;  // offset in tick all'interno del sub-frame
    uint32_t       ulEnd_time;    // deadline (solo per HARD)
    uint32_t       ulSubframe_id; // indice sub-frame
} TimelineTaskConfig_t;

/* Configurazione globale della timeline */
typedef struct {
    uint32_t ulMajorFrameLength;   // durata in tick (es. 100)
    uint32_t ulNumSubframes;       // es. 10
    uint32_t ulSubframeLength;     // es. 10
    const TimelineTaskConfig_t* tasks;
    uint32_t ulNumTasks;
} TimelineConfig_t;

/* Funzione di configurazione da chiamare nel main */
void vConfigureScheduler(const TimelineConfig_t *pxCfg);

#endif /* SCHEDULER_H */