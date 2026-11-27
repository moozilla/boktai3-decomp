#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 PaletteCycleBackground_Init(void *);
void Task_Kill(void *);
void ScriptValues_TickRotation(void);
void BackgroundAnimation_Destroy(void);

void *PaletteCycleBackground_Create(void)
{
    void *r = Task_Create(8, 0x7c);
    if (r != 0) {
        Task_SetCallbacks(r, ScriptValues_TickRotation, BackgroundAnimation_Destroy);
        if (PaletteCycleBackground_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
