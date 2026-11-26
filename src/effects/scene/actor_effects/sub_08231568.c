#define EffectGroup_UpdateDisabled sub_08231444
#include "global.h"
struct EffectGroupEntry {
    u8 state, fadeTimer;
    u16 frame;
    u8 unknown4[0xc];
    u32 flags;
    u8 unknown14[0x34];
};
struct EffectGroup {
    u8 active, mode, cursor, count;
    u8 unknown4[8];
    struct EffectGroupEntry entries[4];
    void (*update)(struct EffectGroup *);
};
void sub_08231444(struct EffectGroup *);
s32 EffectGroup_Reset(struct EffectGroup *group)
{
    struct EffectGroupEntry *entry;
    s32 i;
    group->cursor = 0;
    group->count = 0;
    group->mode = 0;
    group->update = sub_08231444;
    {
        u32 zero = 0;
        u32 timer = 10;
        u32 mask = 1;
        entry = group->entries;
        i = 3;
        do {
            entry->state = zero;
            entry->frame = zero;
            entry->fadeTimer = timer;
            entry->flags |= mask;
            entry++;
            i--;
        } while (i >= 0);
    }
}
