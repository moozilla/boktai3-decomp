#include "global.h"

#ifdef NONMATCHING
// fuzzy: 96.7%
// foundry draft (worker best 96.7%); what differs is mostly register allocation and instruction order
#define EffectGroup_FadeFirstActive sub_082314C4
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
static inline u8 FadeFirst(struct EffectGroup *group)
{
    s32 i = 0;
    u16 zero = 0;
    u8 state = 2;
    struct EffectGroupEntry *entry = group->entries;
    for (; i <= 3; i++) {
        u8 active = entry->state;
        if (active == 1) {
            entry->state = state;
            entry->fadeTimer = 10;
            entry->frame = zero;
            return active;
        }
        entry++;
    }
    return 0;
}
s32 sub_082314C4(struct EffectGroup *group)
{
    if (FadeFirst(group))
        group->count--;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082314C4);
#endif
