#include "global.h"
#define TimedEffect_ReactToContact sub_081DA048
#define TimedEffect_SetCallback sub_081DAD0C
struct EffectContact { u8 unk00[10]; u16 flags; };
struct TimedEffectConfig { u16 flags; u8 unk02[0x48-2]; u32 state; };
struct TimedEffect { u8 unk00[0x18]; struct TimedEffectConfig config; };
void sub_081DAD0C(void *, void *);
void sub_081D9904(void);
void sub_081D934C(void);
void sub_081D7C68(struct TimedEffect *);
void sub_081DA048(struct EffectContact *a, struct EffectContact *b, struct TimedEffect *effect)
{
    struct TimedEffectConfig *config = &effect->config;
    if (config->state == 3) {
        config->flags = 0x80 | config->flags;
        sub_081DAD0C(config, sub_081D9904);
        return;
    }
    if (config->state == 8) return;
    if (config->state == 9) {
        sub_081DAD0C(config, sub_081D934C);
        return;
    }
    {
        u32 af = a->flags;
        u32 one = 1;
        u32 first = one & af;
        u32 bf = b->flags;
        if (first || (one & bf)) {
            if (!(0x20 & bf)) {
                config->flags |= 0x80;
                sub_081D7C68(effect);
            }
        }
    }
}
