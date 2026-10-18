#include "global.h"
struct TimedEffect {
    u8 unk00[0x1AC];
};
struct TimedEffectManager {
    u8 unk00[0x18];
    u32 activeMask;
    u8 unk1C[0x3C - 0x1C];
    s32 capacity;
    u8 unk40[0x108 - 0x40];
    struct TimedEffect *entries;
};
struct TimedEffect *TimedEffect_FindFreeEntry(struct TimedEffectManager *manager, s32 *slot)
{
    struct TimedEffect *entry = manager->entries;
    s32 i;
    for (i = 0; i < manager->capacity; i++, entry++) {
        if (!(manager->activeMask & (1 << i))) {
            *slot = i;
            return entry;
        }
    }
    return 0;
}
