#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"

struct RatatoskParticlePool *Ratatosk_CreateAnimatedParticlePool(void)
{
    struct RatatoskParticlePool *pool = (void *)Task_Create(8, sizeof(struct RatatoskParticlePool));
    if (pool) {
        Task_SetCallbacks(&pool->task, (void *)Ratatosk_UpdateAnimatedParticlePool,
                          (void *)Ratatosk_OnAnimatedParticlePoolDestroy);
        if (Ratatosk_InitAnimatedParticlePool(pool) < 0) {
            Task_Kill(&pool->task);
            return 0;
        }
        return pool;
    }
}
