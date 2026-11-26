#ifndef GUARD_MUSPELL_HEAD_H
#define GUARD_MUSPELL_HEAD_H

#include "engine/math/scene_vec.h"
#include "scene/collision/contact_swarm.h"

struct MuspellHead {
    struct Task task;
    s32 age;
    struct SceneVec origin;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider passiveCollider, bodyCollider;
    u8 body[0x48];
    void (*action)(void);
    struct DisplaySequenceState playback;
    s32 attackDistance;
    s16 attackA, attackB, attackC;
    u8 unknown17A[2];
    s16 health;
    u8 unknown17E[2];
    s16 damage;
    u16 unknown182, timer, dead, unknown188, active, flash, duration;
    s16 enabled, delayA, delayB, phase, maximumHealth;
    u16 damageType, damageFlags;
    s16 hudMode, overlay, hudValue, optionA, optionB, initialDead;
    u8 unknown1AA[2];
    void *parent;
    u32 optionD, previousHit, currentHit, optionG0, optionG1, optionG2, terrain;
    void *sequence;
    u8 cell[0x10];
};

typedef char MuspellHeadSizeCheck[sizeof(struct MuspellHead) == 0x1E0 ? 1 : -1];

#endif
