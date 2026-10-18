#include "global.h"
struct TimedEffectEntry {
    u32 flags;
    u8 unknown[0x54];
    u16 timer;
    u8 tail[2];
};
struct TimedEffect {
    u8 unknown[0x3C];
    struct TimedEffectEntry entries[2];
};
void TimedEffect_UpdateVisibility(struct TimedEffect *effect)
{
    s32 i = 0;
    do {
        if (effect->entries[i].timer) {
            if (--effect->entries[i].timer == 0)
                effect->entries[i].flags = (effect->entries[i].flags | 1) & ~2;
        }
        i++;
    } while (i <= 1);
}
