#ifndef GUARD_AXE_ARMOR_H
#define GUARD_AXE_ARMOR_H

#include "scene/actor/scene_actor_components.h"
#include "scene/collision/contact_swarm.h"

struct AxeArmor {
    struct Task task;
    s32 timer;
    struct ContactPosition initialPosition;
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider collider;
    struct SceneActorBody body;
    s32 (*update)(struct AxeArmor *);
    struct DisplaySequenceState sequence;
    u8 unknown11C[4];
    s16 attackA, attackB, attackC;
    s16 health, pendingDamage, hitFlash;
    s16 countdown, dead, flashTimer;
    s8 facing;
    u8 unknown133;
    s16 interval, attackD, maxHealth;
    s16 rangeA, rangeB, hudStyle, bodyEnabled, hudTime, initialDead;
    u8 unknown146[2];
    s16 unknown148, unknown14A;
    u8 *attackSchedule;
    u32 valueD, terrainEnabled;
    void *resource, *parent;
    u8 terrain[0x10];
    u32 valueG0, valueG1, valueG2;
    u32 displayedDamageType, damageType;
    s16 attackCount, attackIndex;
    s16 attackTimes[8];
};

typedef char AxeArmorSize[(sizeof(struct AxeArmor) == 0x198) ? 1 : -1];

#endif
