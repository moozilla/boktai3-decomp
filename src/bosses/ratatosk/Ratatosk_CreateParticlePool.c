#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "engine/ram.h"

s32 Ratatosk_UpdateParticlePool(struct ParticlePool *);
u32 RatatoskParticle_Destroy(struct ParticlePool *);
s32 RatatoskParticle_Init(struct ParticlePool *);

struct ParticlePool *Ratatosk_CreateParticlePool(void)
{
    struct ParticlePool *pool = (void *)Task_Create(8, sizeof(struct ParticlePool));
    if (pool) {
        Task_SetCallbacks(&pool->task, (void *)Ratatosk_UpdateParticlePool,
                          (void *)RatatoskParticle_Destroy);
        if (RatatoskParticle_Init(pool) < 0) {
            Task_Kill(&pool->task);
            return 0;
        }
        return pool;
    }
}
