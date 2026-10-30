#ifndef GUARD_DISPLAY_SEQUENCE_H
#define GUARD_DISPLAY_SEQUENCE_H

#include "global.h"

/* Each command stores duration weight in bits 0..3, flips in bits 4..5,
 * and the sprite frame number in bits 6..15. */
struct DisplaySequenceState {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};

/* Sequence state embedded at +0x48 in the actor's sprite drawable. */
struct SceneActorControlMotionDrawable {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0x36];
    struct DisplaySequenceState sequence;
};

/* A control byte is also passed around as a packed value. Keep the stored
 * DisplaySequenceState.flags member byte-compatible with existing users. */
union DisplaySequenceControl {
    u8 raw;
    struct {
        u8 flipX : 1;
        u8 flipY : 1;
        u8 backwards : 1;
        u8 hold : 1;
        u8 unused : 4;
    } __attribute__((packed)) bits;
} __attribute__((packed));

typedef char DisplaySequenceControlSizeCheck[(sizeof(union DisplaySequenceControl) == 1) ? 1 : -1];

#endif
