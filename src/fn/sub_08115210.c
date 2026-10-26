#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 EffectPool_InitManager(u8 *);
void Task_Kill(u8 *);
void EffectPool_UpdateEntries(void);
void NativeParticle_DestroyManager(void);

u8 *EffectPool_CreateManager(void)
{
    u8 *p;
    if (gParticleManager != 0)
        return gParticleManager;
    p = Task_Create(0xa, 0xaa0);
    if (p != 0) {
        Task_SetCallbacks(p, EffectPool_UpdateEntries, NativeParticle_DestroyManager);
        if (EffectPool_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
