#include "global.h"
struct EffectGroupEntry {
    u8 state, fadeTimer;
    u16 frame;
    u8 unknown4[0x44];
};
struct EffectGroup {
    u8 active, unknown1[2], count;
    u8 unknown4[8];
    struct EffectGroupEntry entries[4];
};
s32 EffectGroup_FadeFirstActive(struct EffectGroup *group)
{
    s32 found = 0;
    s32 i = 0;
    u16 zero = 0;
    u8 fading = 2;
    struct EffectGroupEntry *entry = group->entries;
    for (; i <= 3; entry++, i++) {
        if (entry->state == 1) {
            entry->state = fading;
            entry->fadeTimer = 10;
            entry->frame = zero;
            found = 1;
            break;
        }
    }
    if (found)
        group->count--;
    return 0;
}
