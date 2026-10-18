#include "global.h"

extern u8 *gUnk_02000194;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ParticleEffectPool_Init(u8 *);
void Task_Kill(u8 *);
void sub_0812D8EC(void);
void ParticleEffectPool_DestroyTask(void);

u8 *ParticleEffectPool_GetOrCreate(void)
{
    u8 *p;
    if (gUnk_02000194 != 0)
        return gUnk_02000194;
    p = Task_Create(0xa, 0x6a0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812D8EC, ParticleEffectPool_DestroyTask);
        if (ParticleEffectPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
