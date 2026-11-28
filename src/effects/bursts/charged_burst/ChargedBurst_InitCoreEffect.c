#include "effects/bursts/charged_burst.h"

void ParticleSprite_Initialize(struct ChargedBurstSprite *, void *, s32);
void Effect_SetOrigin(struct ChargedBurstSprite *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct ChargedBurstSprite *, void *, s32);

void ChargedBurst_InitCoreEffect(struct ChargedBurst *burst)
{
    struct ChargedBurstSprite *core = &burst->core;

    burst->coreResource = AnimData_FindEntry(0x1c1a);
    ParticleSprite_Initialize(core, burst->coreResource, 1);
    Effect_SetOrigin(core, -8, -8);
    SpriteDrawable_SelectTileFrame(core, burst->coreResource, 10);
    core->position = burst->position;
    core->position.y += 215;
    core->unk_0F = 1;
    core->unk_10 = 20;
}
