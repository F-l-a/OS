#include "scheduler.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define MAX_HRT_PER_SUBFRAME   8
#define MAX_SRT_PER_SUBFRAME   8

typedef struct {
    const TimelineTaskConfig_t* pxHrtTasks[MAX_HRT_PER_SUBFRAME];
    uint8_t                     ucNumHrt;
    const TimelineTaskConfig_t* pxSrtTasks[MAX_SRT_PER_SUBFRAME];
    uint8_t                     ucNumSrt;
} SubframeSchedule_t;

static TimelineConfig_t g_cfg;
static SubframeSchedule_t *g_subframes = NULL;
static TaskHandle_t xSchedulerHandle = NULL;

static void vTimelineSchedulerTask(void *params); // dichiarata dopo

void vConfigureScheduler(const TimelineConfig_t *pxCfg)
{
    // Copio la configurazione
    g_cfg = *pxCfg;

    // Alloco array sub-frame (se statico, potrebbe essere un array fisso)
    g_subframes = pvPortMalloc(g_cfg.ulNumSubframes * sizeof(SubframeSchedule_t));
    configASSERT(g_subframes != NULL);
    memset(g_subframes, 0, g_cfg.ulNumSubframes * sizeof(SubframeSchedule_t));

    // Smisto i task nei loro sub-frame
    for (uint32_t i = 0; i < g_cfg.ulNumTasks; i++) {
        const TimelineTaskConfig_t *t = &g_cfg.tasks[i];
        SubframeSchedule_t *sf = &g_subframes[t->ulSubframe_id];

        if (t->type == TASK_HARD_RT) {
            sf->pxHrtTasks[sf->ucNumHrt++] = t;
        } else {
            sf->pxSrtTasks[sf->ucNumSrt++] = t;
        }
    }

    // Crea il task dello scheduler
    xTaskCreate(
        vTimelineSchedulerTask,
        "TimelineSched",
        configMINIMAL_STACK_SIZE * 3,
        NULL,
        configMAX_PRIORITIES - 1,     // priorità alta
        &xSchedulerHandle
    );
}

static void prvRunHrtTasks(uint32_t sfIdx, TickType_t xSubStart);
static void prvRunSrtTasks(uint32_t sfIdx, TickType_t xSubStart, TickType_t xSubEnd);

static void vTimelineSchedulerTask(void *params)
{
    TickType_t xFrameStart = xTaskGetTickCount();

    for (;;) {
        for (uint32_t sf = 0; sf < g_cfg.ulNumSubframes; sf++) {

            TickType_t xSubStart = xFrameStart + sf * g_cfg.ulSubframeLength;
            TickType_t xSubEnd   = xSubStart + g_cfg.ulSubframeLength;

            /* 1) Aspetta l'inizio del sub-frame */
            vTaskDelayUntil(&xSubStart, 0); // “wait until time == xSubStart”

            /* 2) Esegui HRT */
            prvRunHrtTasks(sf, xSubStart);

            /* 3) Esegui SRT nel tempo residuo */
            prvRunSrtTasks(sf, xSubStart, xSubEnd);
        }

        /* 4) Fine major frame: prepara il prossimo */
        // Qui potresti fare reset di stato, log, ecc.

        xFrameStart += g_cfg.ulMajorFrameLength;
    }
}

static void prvRunHrtTasks(uint32_t sfIdx, TickType_t xSubStart)
{
    SubframeSchedule_t *sf = &g_subframes[sfIdx];

    for (uint8_t i = 0; i < sf->ucNumHrt; i++) {
        const TimelineTaskConfig_t *cfg = sf->pxHrtTasks[i];

        TickType_t xStart    = xSubStart + cfg->ulStart_time;
        TickType_t xDeadline = xSubStart + cfg->ulEnd_time;

        // Aspetta l'istante di start
        while (xTaskGetTickCount() < xStart) {
            taskYIELD();
        }

        TickType_t before = xTaskGetTickCount();

        // Esecuzione non-preemptive (a livello di FreeRTOS)
        cfg->function(NULL);

        TickType_t after = xTaskGetTickCount();

        if (after > xDeadline) {
            // Violazione deadline → qui potresti loggare o prendere azione
            // (per il progetto basta loggare/contare)
        }
    }
}

static void prvRunSrtTasks(uint32_t sfIdx, TickType_t xSubStart, TickType_t xSubEnd)
{
    SubframeSchedule_t *sf = &g_subframes[sfIdx];

    for (uint8_t i = 0; i < sf->ucNumSrt; i++) {
        const TimelineTaskConfig_t *cfg = sf->pxSrtTasks[i];

        if (xTaskGetTickCount() >= xSubEnd) {
            // Fine tempo disponibile → SRT rimanenti saltati
            return;
        }

        cfg->function(NULL);   // Best-effort, nessuna deadline rigida
    }
}

