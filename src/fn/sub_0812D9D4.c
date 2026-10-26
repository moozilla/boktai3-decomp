#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ParticleEffectPool_Init(u8 *);
void Task_Kill(u8 *);
void ParticleEffectPool_Update(void);
void ParticleEffectPool_DestroyTask(void);

u8 *ParticleEffectPool_GetOrCreate(void)
{
    u8 *p;
    if (gSceneParticlePool != 0)
        return gSceneParticlePool;
    p = Task_Create(0xa, 0x6a0);
    if (p != 0) {
        Task_SetCallbacks(p, ParticleEffectPool_Update, ParticleEffectPool_DestroyTask);
        if (ParticleEffectPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
