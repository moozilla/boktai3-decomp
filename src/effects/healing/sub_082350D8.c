#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void HealthRestoreEffect_OnUpdate(u8 *);
void HealthRestoreEffect_OnDestroy(u8 *);
s32 HealthRestoreEffect_Init(u8 *, u8 *);
u8 *HealthRestoreEffect_Create(u8 *p)
{
    u8 *obj = Task_Create(11, 0x278);
    if (obj) {
        Task_SetCallbacks(obj, HealthRestoreEffect_OnUpdate, HealthRestoreEffect_OnDestroy);
        if (HealthRestoreEffect_Init(obj, p) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
