#ifndef GUARD_PARTICLE_EFFECT_STATE_H
#define GUARD_PARTICLE_EFFECT_STATE_H

#include "contact_swarm.h"

struct BurstPos {
    s32 x, y, z, w;
};
struct BurstEntry {
    void (*update)(struct BurstEntry *);
    u8 collider[0x5c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    void *data;
    struct BurstPos position;
    s32 velocity[4];
    s32 yOffset;
    u8 padC4[4];
    u16 duration;
    u8 active;
    u8 entered;
    u16 id;
    u8 timer;
    u8 state;
    u32 value;
};

struct FollowingPosition {
    s32 x, y, z, w;
};
struct FollowingEffect {
    void (*update)(struct FollowingEffect *);
    u8 collider[0x5c];                /* 0x04 */
    struct ContactSwarmSprite sprite; /* 0x60 */
    struct DisplaySequenceState anim; /* 0x8c */
    void *sequence;
    struct FollowingPosition *anchor;  /* 0xa0 */
    struct FollowingPosition position; /* 0xa4 */
    u8 unk_B4[0x10];
    s32 *height; /* 0xc4 */
    u8 unk_C8[4];
    u16 duration;
    u8 active, entered;
};

#endif
