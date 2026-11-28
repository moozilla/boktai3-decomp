#include "effects/bursts/linked_effect.h"

s32 FragmentProjectilePool_Init(struct LinkedEffectPool *);
s32 ActorEffect_UpdatePool(struct LinkedEffectPool *);
u32 FragmentProjectilePool_Cleanup(struct LinkedEffectPool *);

struct LinkedEffectPool *FragmentProjectilePool_GetOrCreate(void)
{
    struct LinkedEffectPool *pool;

    if (gActorEffectManager)
        return (struct LinkedEffectPool *)gActorEffectManager;
    pool = Task_Create(0x8, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, ActorEffect_UpdatePool, FragmentProjectilePool_Cleanup);
        if (FragmentProjectilePool_Init(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
