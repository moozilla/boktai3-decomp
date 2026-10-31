#ifndef GUARD_SCREAMER_STATE_H
#define GUARD_SCREAMER_STATE_H

#include "display_animation.h"
struct ActorPosition {
    s16 x, y, z, pad;
};
struct ScreamerSprite {
    u32 flags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct ActorPosition position;
    u8 unknown24[8];
};
/* Private state begins before the end of the canonical actor record. */
struct ScreamerState {
    u8 unknown0[24];
    u32 age;
    struct ActorPosition initialPosition;
    struct ScreamerSprite sprite;
    u8 graphics[0x1c];
    u8 attackCollider[0x54];
    u8 bodyCollider[6];
    u16 colliderFlags;
    u8 unknownC8[82];
    u16 contactFlags;
    u8 unknown11C[76];
    u8 body[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    s16 f1C4;
    s16 f1C6;
    s16 f1C8;
    s16 f1CA;
    s16 f1CC;
    s16 f1CE;
    s16 f1D0;
    s16 f1D2;
    s16 f1D4;
    s16 f1D6;
    s16 f1D8;
    s16 f1DA;
    s16 f1DC;
    s16 f1DE;
    s16 f1E0;
    s16 f1E2;
    s16 f1E4;
    s16 f1E6;
    s16 f1E8;
    s16 f1EA;
    s16 f1EC;
    s16 f1EE;
    s16 f1F0;
    s16 f1F2;
    s16 f1F4;
    s16 f1F6;
    void *f1F8;
    u32 f1FC;
    u32 f200;
    u32 f204;
    u32 f208;
    u32 f20C;
    u32 f210;
    u32 f214;
    void *f218;
    u32 f21C;
};

#endif
