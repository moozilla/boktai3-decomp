#ifndef GUARD_WIND_CANNON_H
#define GUARD_WIND_CANNON_H

#include "scene/collision/contact_swarm.h"

struct FanCannon {
    s32 age;
    s32 name;
    struct ContactPosition initial;
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1c];
    struct ContactCollider collider;
    u8 body[0x48];
    void (*update)(void);
    struct DisplaySequenceState sequence;
    s32 range;
    s16 maximumWind, health, damage, reaction;
    s16 f114, dead, flash;
    s8 direction;
    u8 reverse;
    s16 period, palette, wind, variant;
    u8 activated, stopped;
    s16 hasBody;
    s16 maximumHealth, hudA, hudB, f12e, f130, f132, hudTime, initialDead;
    void *schedule;
    s32 f13c, terrain;
    void *resource;
    u8 terrainCell[8];
    s32 f150, f154, f158, f15c, f160, f164, f168;
    s16 scheduleCount, scheduleIndex;
    s16 times[8];
};

struct FanCannonManager {
    struct Task task;
    u32 occupiedSlots;
    struct FanCannon slots[8];
};

typedef char FanCannonSizeCheck[sizeof(struct FanCannon) == 0x180 ? 1 : -1];
typedef char FanCannonManagerSizeCheck[sizeof(struct FanCannonManager) == 0xC1C ? 1 : -1];

#endif
