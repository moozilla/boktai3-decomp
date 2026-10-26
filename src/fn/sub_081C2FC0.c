#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 SparkleEmitter_Initialize(void *, void *);
void Task_Kill(void *);
void SparkleEmitter_OnTaskUpdate(void *);
void SparkleEmitter_OnTaskDestroy(void *);
void *SparkleEmitter_CreateFromScript(void *a)
{
    void *p;
    if (gSparkleEmitter != 0)
        return gSparkleEmitter;
    p = gSparkleEmitter = Task_Create(8, 0x780);
    if (p != 0) {
        Task_SetCallbacks(p, SparkleEmitter_OnTaskUpdate, SparkleEmitter_OnTaskDestroy);
        if (SparkleEmitter_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
