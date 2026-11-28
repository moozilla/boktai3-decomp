#ifndef GUARD_PLAYER_PARTICLE_CLOUD_H
#define GUARD_PLAYER_PARTICLE_CLOUD_H

#include "global.h"
#include "engine/task.h"

struct CloudPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct CloudSprite {
    u32 flags;
    u8 pad4[0x14];
    struct CloudPosition position;
    u8 pad20[8];
};
struct CloudParticle {
    u8 kind, frame, timer, duration, angle, speed;
    u16 delay, lifetime, padA;
    s16 motionX, motionY, motionZ;
    u16 pad12;
    struct CloudPosition origin;
    struct CloudPosition *anchor;
    struct CloudSprite sprite;
};
struct CloudPlayer {
    u8 pad0[0x20];
    u32 statusFlags;
    u8 pad24[0xC];
    struct CloudPosition position;
};
struct CloudTask {
    struct Task task;
    u32 ticks;
    u16 mode;
    u8 duration, stop;
    u32 frameBase;
    struct CloudPosition anchor;
    void *sheet;
    struct CloudParticle particles[40];
    struct CloudPlayer *player;
};

#endif
