#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ActorParticles_InitPool(u8 *);
void Task_Kill(u8 *);
void CallbackPool_InvokeActive(void);
void Collectible_DestroyManager(void);

u8 *Collectible_CreateManager(void)
{
    u8 *p;
    if (gActorParticlePool != 0)
        return gActorParticlePool;
    p = Task_Create(0x8, 0x2e98);
    if (p != 0) {
        Task_SetCallbacks(p, CallbackPool_InvokeActive, Collectible_DestroyManager);
        if (ActorParticles_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
