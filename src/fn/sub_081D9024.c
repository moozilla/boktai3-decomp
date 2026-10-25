#include "global.h"
#define TimedEffect_SetCallback Projectile_SetUpdateCallback
struct TimedEffectConfig {
    u16 flags;
    u16 duration;
};
struct TimedEffectPhase {
    u8 unk00[0x10];
    u16 phase;
    u8 unk12[0x1C - 0x12];
};
struct TimedEffect {
    u8 unk00[0x18];
    struct TimedEffectConfig config;
    u8 unk1C[0x160 - 0x1C];
    u32 timer;
};
void Projectile_SetUpdateCallback(void *, void *);
void TimedEffect_UpdateHoming(void);
s32 TimedEffect_BeginPhase26(struct TimedEffect *effect)
{
    struct TimedEffectConfig *config = &effect->config;
    struct TimedEffectPhase *phase;
    Projectile_SetUpdateCallback(config, TimedEffect_UpdateHoming);
    phase = (void *)((u8 *)effect + 0x8C);
    {
        u32 phaseId = 0x1A;
        u32 zero = 0;
        phase->phase = phaseId;
        effect->timer = zero;
    }
    if (config->duration < 120)
        config->duration = 120;
    return 0;
}
