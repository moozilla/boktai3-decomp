#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"
#include "engine/ram.h"

void OverlaySlot_Initialize(struct ParticlePool *, struct Particle *, s32);
void AnimData_Load(void *, u32);
void *ScriptProc_Start(u32, u32);

u32 RatatoskParticle_Init(struct ParticlePool *pool)
{
    struct Particle *e;
    s32 i;
    gParticlePool = pool;
    pool->mask = 0;
    e = pool->particles;
    for (i = 0; i < 24; i++, e++)
        OverlaySlot_Initialize(pool, e, i);
    AnimData_Load(&pool->graphics, 0x8E5A);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x4363);
    return 0;
}
