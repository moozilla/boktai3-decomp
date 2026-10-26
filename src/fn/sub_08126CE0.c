#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 BurstSpark_InitPool(u8 *);
void Task_Kill(u8 *);
void EffectPool_UpdateActive(void);
void BurstSpark_DestroyPool(void);

u8 *BurstSpark_EnsurePool(void)
{
    u8 *p;
    if (gBurstSparkPool != 0)
        return gBurstSparkPool;
    p = Task_Create(0xa, 0x2038);
    if (p != 0) {
        Task_SetCallbacks(p, EffectPool_UpdateActive, BurstSpark_DestroyPool);
        if (BurstSpark_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
