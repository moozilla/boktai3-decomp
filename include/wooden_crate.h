#ifndef GUARD_WOODEN_CRATE_H
#define GUARD_WOODEN_CRATE_H

#include "display_sequence.h"
#include "contact_position.h"

struct WoodenCrate {
    u32 flags;
    u8 pad4[3];
    u8 priority;
    u8 pad8[4];
    void *drawable;
    u16 frame;
    u8 pad12[10];
    struct ContactPosition position;
    u8 pad24[8];
    struct DisplaySequenceState animation;
    u8 resource[0x1C];
    struct ContactPosition origin;
    u8 pad60[0x54];
    u32 name, toggleScript, releaseScript;
    u8 padC0[4];
    u16 state, index, crateFlags;
    s16 palette;
    u8 sequenceFlags, flashTimer;
    u16 originalFlags, triggerTime, triggerCount;
    u8 changed;
};

#endif
