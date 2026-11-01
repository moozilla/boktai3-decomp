#include "effect_group.h"
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
s32 EffectGroup_SpawnPart(struct EffectGroup *group, s32 count,
                          const struct ContactPosition *position,
                          const struct ContactPosition *velocity,
                          const struct ContactPosition *spread, s32 lifetime, s32 lifetimeMask)
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
            sub_08014634(count, group->mode, position, velocity, spread, lifetime, lifetimeMask);
            break;
        }
    }
    if (spawned) {
        group->count--;
        group->cursor--;
    }
    return 0;
}
