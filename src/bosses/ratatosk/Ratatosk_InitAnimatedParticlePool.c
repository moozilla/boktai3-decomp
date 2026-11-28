#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "engine/ram.h"

s32 AnimData_Load(struct SpriteRenderResource *, u32);
void *ScriptProc_Start(u16, u16);
s32 Ratatosk_InitAnimatedParticlePool(struct RatatoskParticlePool *pool)
{
    struct RatatoskParticle *e;
    s32 i;
    gRatatoskParticlePool = pool;
    pool->mask = 0;
    e = pool->particles;
    for (i = 0; i < 32; i++, e++)
        OverlaySlot_InitializeExtended(pool, e, i);
    AnimData_Load(&pool->graphics, 0x8E5A);
    pool->sequenceResource = ScriptProc_Start(0x922E, 0x4363);
    return 0;
}
