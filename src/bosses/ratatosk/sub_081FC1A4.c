#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "engine/shared/ram.h"

s32 AnimData_Load(struct SpriteRenderResource *, u32);
void *ScriptProc_Start(u16, u16);
s32 Ratatosk_InitAnimatedParticlePool(struct RatatoskParticlePool *a)
{
    struct RatatoskParticle *e;
    s32 i;
    gRatatoskParticlePool = a;
    a->mask = 0;
    e = a->particles;
    i = 0;
    do {
        OverlaySlot_InitializeExtended(a, e, i);
        i++;
        e++;
    } while (i <= 0x1f);
    AnimData_Load(&a->graphics, 0x8E5A);
    a->sequenceResource = ScriptProc_Start(0x922E, 0x4363);
    return 0;
}
