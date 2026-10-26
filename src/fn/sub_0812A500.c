#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SunDiscPool_Init(u8 *);
void Task_Kill(u8 *);
void EffectPool_UpdateSlots(void);
void SunDiscPool_DestroyTask(void);

u8 *SunDiscPool_GetOrCreate(void)
{
    u8 *p;
    if (gSunDiscPool != 0)
        return gSunDiscPool;
    p = Task_Create(0x8, 0xc10);
    if (p != 0) {
        Task_SetCallbacks(p, EffectPool_UpdateSlots, SunDiscPool_DestroyTask);
        if (SunDiscPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
