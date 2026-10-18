#include "global.h"
#define TimedEffect_TestDuration sub_081D8FE8
struct TimedEffectConfig { u16 flags; u16 duration; };
struct TimedEffect { u32 tick; u32 flags; u8 unk08[16]; struct TimedEffectConfig config; };
s32 sub_081D8FE8(struct TimedEffect *effect)
{
    struct TimedEffectConfig *config = &effect->config;
    if ((effect->tick & 1) + 1 == ((config->flags & 0xF00) >> 8))
        { u32 mask = 2; effect->flags |= mask; }
    if (effect->tick >= config->duration)
        return 2;
    return 0;
}
