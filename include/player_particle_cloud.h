#ifndef GUARD_PLAYER_PARTICLE_CLOUD_H
#define GUARD_PLAYER_PARTICLE_CLOUD_H

#include "global.h"

struct CloudPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct CloudSprite {
    u8 pad0[0x18];
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
    u8 pad0[0x30];
    struct CloudPosition position;
};
struct CloudTask {
    u8 pad0[0x18];
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
