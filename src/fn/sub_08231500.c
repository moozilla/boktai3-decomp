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
};
void sub_08014634(u32, u32, u32, u32, u32, u32, u32);
#define EffectGroup_SpawnPart sub_08231500
s32 EffectGroup_SpawnPart(struct EffectGroup *group, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f)
{
    struct EffectGroupEntry *entry;
    s32 spawned = 0;
    s32 i = 0;
    u32 state;
    u32 zero = 0;
    entry = group->entries;
    for (; i <= 3; entry++, i++) {
        state = entry->state;
        if (state == 1) {
            entry->flags |= state;
            entry->state = zero;
            entry->fadeTimer = 10;
            entry->frame = zero;
            spawned = 1;
            sub_08014634(a, group->mode, b, c, d, e, f);
            break;
        }
    }
    if (spawned) {
        group->count--;
        group->cursor--;
    }
    return 0;
}
