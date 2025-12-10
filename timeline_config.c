#include "timeline_config.h"
#include "tasks_app.h"

/* Esempio: major frame 100 tick, 4 sub-frame da 25 tick */
#define MAJOR_FRAME_TICKS   100
#define NUM_SUBFRAMES       4
#define SUBFRAME_TICKS      25

/* Tabella dei task nella timeline */
static const TimelineTaskConfig_t g_timelineTasks[] = {
    //  name,          function,            type,          start, end, subframe
    { "ReadSensor",    vTaskReadSensor,    TASK_HARD_RT,   0,   10,   0 },
    { "ControlLoop",   vTaskControlLoop,   TASK_HARD_RT,  10,   20,   0 },
    { "Logging",       vTaskLogging,       TASK_SOFT_RT,   0,   25,   0 },
    { "ReadSensor",    vTaskReadSensor,    TASK_HARD_RT,   0,   10,   1 },
    { "Diagnostics",   vTaskDiagnostics,   TASK_SOFT_RT,   0,   25,   1 }
};

const TimelineConfig_t g_timelineConfig = {
    .ulMajorFrameLength = MAJOR_FRAME_TICKS,
    .ulNumSubframes     = NUM_SUBFRAMES,
    .ulSubframeLength   = SUBFRAME_TICKS,
    .tasks              = g_timelineTasks,
    .ulNumTasks         = sizeof(g_timelineTasks) / sizeof(g_timelineTasks[0])
};