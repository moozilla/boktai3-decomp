#ifndef GUARD_ARMOR_PART_QUEUE_H
#define GUARD_ARMOR_PART_QUEUE_H

#include "global.h"

/* Partial views of the three embedded Armor parts. */
struct ArmorLaunchPosition {
    s16 x, y, z, pad;
};
struct ArmorLaunchPart {
    u8 unk_00[0x1C];
    struct ArmorLaunchPosition position;
    u8 unk_24[0x119 - 0x24];
    u8 flags;
    u8 unk_11A[4];
    u8 active, action;
    u8 unk_120[0x134 - 0x120];
    struct ArmorLaunchPosition target;
    u8 unk_13C[0x1E4 - 0x13C];
};
struct ArmorLaunchState {
    u8 flags, unk_01[4], active, action;
    u8 unk_07[0x1E4 - 7];
};
struct ArmorLaunchPositionSlot {
    struct ArmorLaunchPosition position;
    u8 unk_08[0x1E4 - 8];
};
struct ArmorLaunchGroup {
    struct ArmorLaunchPart parts[3];
    struct ArmorLaunchPosition position;
    u8 unk_5B4[0x5C3 - 0x5B4];
    u8 count, activeCount;
};

/* Activate a part, signal its next action, and count it in the group. */
#define ARMOR_QUEUE_PART_ACTION(state_, next_, active_, bit_, count_) \
    do { \
        u8 pending = (bit_); \
        (state_)->active = (active_); \
        (state_)->action = (next_); \
        (state_)->flags = pending | (state_)->flags; \
        ++*(count_); \
    } while (0)

#endif
