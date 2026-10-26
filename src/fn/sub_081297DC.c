#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FeatherEffectPool_Init(u8 *);
void Task_Kill(u8 *);
void FeatherEffectPool_UpdateTask(void);
void FeatherEffectPool_DestroyTask(void);

u8 *FeatherEffectPool_GetOrCreate(void)
{
    u8 *p;
    if (gFadingSpritePool != 0)
        return gFadingSpritePool;
    p = Task_Create(0x8, 0xcbc);
    if (p != 0) {
        Task_SetCallbacks(p, FeatherEffectPool_UpdateTask, FeatherEffectPool_DestroyTask);
        if (FeatherEffectPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
