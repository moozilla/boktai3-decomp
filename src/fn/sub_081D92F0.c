#include "global.h"
#define TimedEffect_SetCallback Projectile_SetUpdateCallback
struct TimedEffectConfig {
    u16 flags, duration;
};
struct TimedEffect {
    u32 tick;
    u8 unk04[0x18 - 4];
    struct TimedEffectConfig config;
    u8 unk1C[0xAA - 0x1C];
    s16 position;
    u8 unkAC[0x164 - 0xAC];
    s32 acceleration, velocity;
    u8 unk16C[0x17E - 0x16C];
    s16 limit;
};
void Projectile_SetUpdateCallback(void *, void *);
void sub_081D934C(void);
s32 TimedEffect_UpdateDeceleration(struct TimedEffect *effect)
{
    struct TimedEffectConfig *config = &effect->config;
    effect->velocity -= effect->acceleration;
    effect->acceleration--;
    if (effect->tick >= config->duration || effect->position < 0 ||
        effect->position < effect->limit)
        Projectile_SetUpdateCallback(config, sub_081D934C);
    return 0;
}
