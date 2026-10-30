#include "global.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_components.h"
#include "contact_collider.h"

struct ArmorDecisionActor {
    u8 task[0x18];
    u16 taskFlags;
    u8 unknown1A[0x3C - 0x1A];
    struct SceneActorBody body;
    u8 unknown84[0x28C - 0x84];
    struct ContactCollider collision;
    u8 unknown2E0[0xB32 - 0x2E0];
    u16 flags;
    u8 unknownB34[4];
    u8 type, facing;
    u8 unknownB3A[2];
    u8 behavior, phase;
    u8 unknownB3E[0xB58 - 0xB3E];
    u8 sequence[0x1120 - 0xB58];
    u8 decision, fastMode;
    u16 attackCooldown, secondaryCooldown, collisionCooldown, modeTimer;
};
void ActorController_SetBehavior(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);

static inline void Armor_ClearCollision(struct ContactCollider *collision, u32 bits)
{
    u32 mask = ~bits;
    collision->flags &= mask;
}
static inline void Armor_SetCollision(struct ContactCollider *collision)
{
    collision->flags |= 4;
}
static inline u16 Armor_NextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * sizeof(u16);
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

static inline u8 Armor_FacingValue(struct ArmorDecisionActor *actor)
{
    return actor->facing;
}

#define ARMOR_ROUND_GRID_COMPONENT(value)                                                          \
    do {                                                                                           \
        (value) = ((value) + 128) >> 8;                                                            \
    } while (0)
#define ARMOR_BRANCH_FACING_AXIS(actor, delta, negative, positive, aligned, unaligned)             \
    do {                                                                                           \
        if ((delta) < 0) {                                                                         \
            if ((actor)->facing == (negative))                                                     \
                goto aligned;                                                                      \
        } else if ((actor)->facing == (positive))                                                  \
            goto aligned;                                                                          \
        goto unaligned;                                                                            \
    } while (0)
#define ARMOR_NORMALIZE_MAGNITUDE(magnitude, value)                                                \
    do {                                                                                           \
        if ((value) < 0)                                                                           \
            (magnitude) = -(value);                                                                \
    } while (0)
#define ARMOR_CAPTURE_COMPONENT_DELTA(out, target, actor)                                          \
    do {                                                                                           \
        (out) = (target) - (actor);                                                                \
    } while (0)
#define ARMOR_BRANCH_AXIS_UNALIGNED(x, z, label)                                                   \
    do {                                                                                           \
        if ((x) != 0 && (z) != 0)                                                                  \
            goto label;                                                                            \
    } while (0)
// Selects axis attacks and recovery behavior while updating the current mode timers.
void Armor_UpdateModeAttackDecision(struct ArmorDecisionActor *p)
{
    s32 playerX, actorX, playerZ, actorZ;
    s32 dx, dz, absX, absZ, distance, direction;
    u32 mode;
    u16 timer;
    timer = p->attackCooldown;
    if ((s16)timer > 0) {
        p->attackCooldown = timer - 1;
        if (p->fastMode)
            p->attackCooldown = timer - 2;
    }
    timer = p->collisionCooldown;
    if ((s16)timer > 0) {
        p->collisionCooldown = timer - 1;
        if (p->fastMode)
            p->collisionCooldown = timer - 2;
        if ((s16)p->collisionCooldown > 0)
            goto setCollision;
    }
    if (p->flags & 1)
        Armor_ClearCollision(&p->collision, 4);
    else {
    setCollision:
        Armor_SetCollision(&p->collision);
    }
    if ((s16)--p->modeTimer <= 0) {
        u32 flag = p->flags & 1;
        if (flag) {
            p->fastMode = !p->fastMode;
            p->modeTimer = 300;
        } else
            p->modeTimer = flag;
    }
    {
        u32 flag = p->flags & 1;
        if (!flag)
            p->fastMode = flag;
    }
    if (p->phase != 255 && p->behavior != 19 && p->behavior != 33 && p->behavior != 32 &&
        p->behavior != 34 && p->behavior != 2 && p->behavior != 37)
        return;
    playerX = gGameState->positionX;
    actorX = p->body.position.x;
    ARMOR_CAPTURE_COMPONENT_DELTA(dx, playerX, actorX);
    playerZ = gGameState->positionZ;
    actorZ = p->body.position.z;
    ARMOR_CAPTURE_COMPONENT_DELTA(dz, playerZ, actorZ);
    ARMOR_ROUND_GRID_COMPONENT(dx);
    ARMOR_ROUND_GRID_COMPONENT(dz);
    absX = dx;
    absZ = dz;
    ARMOR_NORMALIZE_MAGNITUDE(absX, dx);
    ARMOR_NORMALIZE_MAGNITUDE(absZ, dz);
    mode = p->flags & 1;
    if (mode) {
        switch (p->decision) {
            case 0:
                if (absX + absZ <= 3) {
                    if (p->fastMode)
                        ActorController_SetBehavior(p, 34);
                    else
                        ActorController_SetBehavior(p, 32);
                    p->decision = 1;
                    p->attackCooldown = 180;
                } else if (Armor_IsSequenceIncomplete(p->sequence))
                    ActorController_SetBehavior(p, 8);
                else if (p->fastMode)
                    ActorController_SetBehavior(p, 33);
                else
                    ActorController_SetBehavior(p, 19);
                break;
            case 1:
                if (p->behavior == 3)
                    goto recover;
                distance = absX + absZ;
                if (distance > 4) {
                    if (p->fastMode)
                        ActorController_SetBehavior(p, 33);
                    else
                        ActorController_SetBehavior(p, 19);
                    p->decision = 0;
                    break;
                }
                direction = Math_Atan2(playerX - actorX, playerZ - actorZ);
                direction = (direction + 32) & 192;
                if (Armor_FacingValue(p) == direction && distance <= 1)
                    goto attack;
                if ((s16)p->attackCooldown <= 0) {
                    if (dx == 0) {
                        ARMOR_BRANCH_FACING_AXIS(p, dz, 192, 64, alignedZ, approach);
                    alignedZ:
                        if (Armor_NextRandom() & 4)
                            goto spin;
                        goto swing;
                    } else if (dz == 0) {
                        ARMOR_BRANCH_FACING_AXIS(p, dx, 128, 0, alignedX, approach);
                    alignedX:
                        if (Armor_NextRandom() & 4) {
                        spin:
                            ActorController_SetBehavior(p, 36);
                        } else {
                        swing:
                            ActorController_SetBehavior(p, 24);
                        }
                        break;
                    }
                }
            approach:
                if (p->fastMode)
                    ActorController_SetBehavior(p, 34);
                else
                    ActorController_SetBehavior(p, 32);
                break;
            case 2:
                if (p->behavior == 15)
                    goto attack;
                if (p->fastMode)
                    ActorController_SetBehavior(p, 34);
                else
                    ActorController_SetBehavior(p, 32);
                p->decision = 1;
                p->attackCooldown = 180;
                break;
        }
    } else {
        u8 *decision = &p->decision;
        switch (*decision) {
            case 0:
                if (absX + absZ <= 4) {
                    ActorController_SetBehavior(p, 37);
                    *decision = 1;
                    p->attackCooldown = 360;
                } else
                    ActorController_SetBehavior(p, 2);
                break;
            case 1:
                if (p->behavior == 3) {
                recover:
                    ActorController_SetBehavior(p, 30);
                    break;
                }
                direction = Math_Atan2(playerX - actorX, playerZ - actorZ);
                direction = (direction + 32) & 192;
                if (Armor_FacingValue(p) == direction) {
                    s32 alignedDistance = absX + absZ;
                    if (alignedDistance > 5) {
                        ActorController_SetBehavior(p, 7);
                        *decision = mode;
                        break;
                    }
                    if (alignedDistance <= 1) {
                    attack:
                        ActorController_SetBehavior(p, 3);
                        break;
                    }
                }
                if ((s16)p->attackCooldown > 0)
                    goto axisApproach;
                ARMOR_BRANCH_AXIS_UNALIGNED(dx, dz, axisApproach);
                ActorController_SetBehavior(p, 36);
                p->attackCooldown = 360;
                break;
            axisApproach:
                ActorController_SetBehavior(p, 37);
                break;
            case 2:
                ActorController_SetBehavior(p, 3);
                *decision = 1;
                p->attackCooldown = 360;
                break;
        }
    }
}
