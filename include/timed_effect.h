#ifndef GUARD_TIMED_EFFECT_H
#define GUARD_TIMED_EFFECT_H

#include "contact_swarm.h"

struct TimedEffectConfig {
    u16 flags, strength, tracking;
    s8 direction;
    u8 turnSpeed;
    s32 speed;
    u32 unknown0C;
    void *callback;
    u32 mode, contactFlags, resourceId, cost, option;
    struct ContactPosition bounds, offset;
    s16 category;
    u16 variant;
    u32 graphics, frame;
    s32 palette, type;
    u8 unknown4C[6];
    u16 collisionFlags;
    u32 unknown54;
};
struct TimedEffectTail {
    s32 elapsed;
    s32 unknown04;
    s32 height;
    struct ContactPosition previous;
    u8 unknown14[0x3E - 0x14];
    s16 cooldown;
    u32 index;
};
struct TimedEffect {
    u32 frame, flags;
    u8 unknown08[8];
    struct ContactPosition velocity;
    struct TimedEffectConfig config;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct ContactCollider colliders[2];
    void *body;
    s32 lifetime;
    struct TimedEffectTail tail;
};
struct TimedEffectManagerSprite {
    u8 data[0x30];
};
struct TimedEffectManager {
    u8 task[0x18];
    u32 activeMask;
    s32 categoryCounts[4], totalCounts[4];
    s32 capacity;
    u8 unknown40[8];
    struct TimedEffectManagerSprite effects[4];
    struct TimedEffect *entries;
};

#endif
