#include "global.h"
#define TimedEffect_RemoveFromManager sub_081DA178
struct TimedEffectManager { u8 unk00[0x18]; u32 activeMask; s32 counts[4]; s32 totalCounts[4]; };
struct TimedEffectConfig { u16 flags; u8 unk02[0x38-2]; s16 kind; };
struct TimedEffectSprite { u32 flags; };
struct TimedEffect { u8 unk00[0x18]; struct TimedEffectConfig config; u8 unk54[0x8C - 0x54]; struct TimedEffectSprite sprite; };
void Task4E01_ClearTrailSprites(void *);
void Sprite_Remove(void *);
void sub_081DA178(struct TimedEffectManager *manager, struct TimedEffect *effect, u32 slot)
{
    struct TimedEffectConfig *config = &effect->config;
    struct TimedEffectSprite *sprite;
    u32 one;
    u32 flags;
    if (!(config->flags & 0x2010))
        manager->counts[config->kind]--;
    manager->totalCounts[config->kind]--;
    Task4E01_ClearTrailSprites(manager);
    sprite = &effect->sprite;
    flags = sprite->flags;
    one = 1;
    sprite->flags = flags | one;
    Sprite_Remove(sprite);
    manager->activeMask &= ~(one << slot);
}
