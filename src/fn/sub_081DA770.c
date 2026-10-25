#include "global.h"

void *ResourceRegistry_GetResource(u32);
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 TimedEffect_InitManager(u8 *, s32);
void Task_Kill(u8 *);
void TimedEffect_UpdateManager(void);
void TimedEffect_DestroyManager(void);
/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by TimedEffect_InitManager; NULL when that fails (tentative). */
u8 *TimedEffect_GetOrCreateManager(s32 capacity)
{
    u8 *p = ResourceRegistry_GetResource(0x4E01);
    if (p)
        return p;
    if (capacity > 32)
        capacity = 32;
    if (capacity < 12)
        capacity = 12;
    p = Task_Create(8, 0x10C);
    if (p) {
        Task_SetCallbacks(p, TimedEffect_UpdateManager, TimedEffect_DestroyManager);
        if (TimedEffect_InitManager(p, capacity) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
