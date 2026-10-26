#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FragmentProjectilePool_Init(u8 *);
void Task_Kill(u8 *);
void ActorEffect_UpdatePool(void);
void FragmentProjectilePool_Cleanup(void);

u8 *FragmentProjectilePool_GetOrCreate(void)
{
    u8 *p;
    if (gActorEffectManager != 0)
        return gActorEffectManager;
    p = Task_Create(0x8, 0xc80);
    if (p != 0) {
        Task_SetCallbacks(p, ActorEffect_UpdatePool, FragmentProjectilePool_Cleanup);
        if (FragmentProjectilePool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
