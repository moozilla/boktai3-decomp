#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneProcParticles_Initialize(u8 *);
void Task_Kill(u8 *);
void CallbackPool_Update(void);
void ParticleBurstEffect_DestroyManager(void);

u8 *ParticleBurstEffect_EnsureManager(void)
{
    u8 *p;
    if (gSceneProcParticles != 0)
        return gSceneProcParticles;
    p = Task_Create(0xa, 0x52c);
    if (p != 0) {
        Task_SetCallbacks(p, CallbackPool_Update, ParticleBurstEffect_DestroyManager);
        if (SceneProcParticles_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
