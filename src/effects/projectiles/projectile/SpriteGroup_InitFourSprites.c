#include "effects/projectiles/timed_effect_manager.h"
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_SetPalette(void *, u32);
void SpriteGroup_InitFourSprites(struct TimedEffectManager *manager, u32 animation)
{
    u8 *active;
    struct TimedEffectTrail *entry;
    s32 i;
    s32 offset;
    manager->nextTrail = 0;
    manager->trailResource = AnimData_FindEntry(0x1C1E);
    offset = -4;
    active = &manager->effects[0].active;
    entry = manager->effects;
    i = 3;
    do {
        ParticleSprite_Initialize(entry, manager->trailResource, 1);
        Effect_SetOrigin(entry, offset, offset);
        SpriteDrawable_SelectTileFrame(entry, manager->trailResource, animation);
        Effect_SetPalette(entry, 1);
        *active = 0;
        active += sizeof(*entry);
        ++entry;
        --i;
    } while (i >= 0);
}
