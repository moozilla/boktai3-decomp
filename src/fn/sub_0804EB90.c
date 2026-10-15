#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void AreaEffect_Tick(void);
void AreaEffect_Destroy(void);
s32 AreaEffect_Initialize(void *, s32, s32);

void *AreaEffect_Create(s32 a, s32 b)
{
    void *p = Task_Create(9, 0xAB4);
    if (p) {
        Task_SetCallbacks(p, AreaEffect_Tick, AreaEffect_Destroy);
        if (AreaEffect_Initialize(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
