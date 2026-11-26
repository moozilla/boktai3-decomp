#ifndef GUARD_STATUE_H
#define GUARD_STATUE_H

#include "engine/task.h"
#include "scene/collision/contact_swarm.h"

struct Statue {
    struct Task task;
    s32 timer;
    struct ContactPosition initialPosition;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider collider;
    u8 terrainBody[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    u32 unknown11C;
    s16 attackX, attackY, attackZ;
    s16 hp, damage;
    u16 hit;
    u8 unknown12C[2];
    s16 dead, flash;
    u8 facing, unknown133;
    s16 attackInterval, unknown136, maxHp, bodyReactionFlags, sparkMask, hudStyle;
    s16 showTerrainBody, hudTime, initialDead, unknown146;
    u32 scriptValues, unknown14C, terrainEnabled;
    void *resource;
    s16 unknown158, unknown15A;
    void *owner;
    u32 hitFlags, colliderHitFlags, unknown168, unknown16C, unknown170;
    u8 terrain[0x24];
};

typedef char StatueSizeCheck[sizeof(struct Statue) == 0x198 ? 1 : -1];

#endif
