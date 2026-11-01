#include "global.h"

#include "engine.h"
#include "game_state.h"

void ActorController_SetBehavior(void *actor, u32 behavior);
void ActorController_SetBehaviorIfFinished(void *actor, u32 behavior);
s32 Armor_IsSequenceIncomplete(void *armor);

struct ArmorChaseActor {
    u8 unk0[0x48];
    s16 x, y, z;
    u8 unk4E[0x28C - 0x4E];
    struct ArmorCollision {
        u8 pad[6];
        s16 flags;
    } collision;
    u8 unk294[0xB32 - 0x294];
    u16 flags;
    u8 unkB34[0xB39 - 0xB34];
    u8 facing;
    u8 unkB3A[2];
    u8 behavior;
    u8 phase;
    u8 unkB3E[0xB58 - 0xB3E];
    u8 armorData[0x1120 - 0xB58];
    u8 decision;
    u8 fastMode;
    u16 attackCooldown;
    u8 unk1124[2];
    u16 collisionCooldown;
    u16 modeTimer;
};

static inline void Armor_ClearCollision(struct ArmorCollision *collision, u32 bits)
{
    u32 mask = ~bits;
    collision->flags &= mask;
}
static inline void Armor_SetCollision(struct ArmorCollision *collision)
{
    collision->flags |= 4;
}

static inline u8 Armor_FacingValue(struct ArmorChaseActor *actor)
{
    return actor->facing;
}

static inline u16 Armor_NextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline void Armor_PickAxisAttack(struct ArmorChaseActor *actor)
{
    if (Armor_NextRandom() & 4)
        ActorController_SetBehavior(actor, 36);
    else
        ActorController_SetBehavior(actor, 24);
    actor->attackCooldown = 180;
}

// Player-relative deltas, x and z rounded to 256-unit grid cells, and their magnitudes.
#define ARMOR_PLAYER_DELTAS(actor, px, ax, pz, az, dx, dy, dz, absX, absY, absZ)                   \
    do {                                                                                           \
        px = gGameState->positionX;                                                                \
        ax = (actor)->x;                                                                           \
        dx = px - ax;                                                                              \
        dy = gGameState->positionY - (actor)->y;                                                   \
        pz = gGameState->positionZ;                                                                \
        az = (actor)->z;                                                                           \
        dz = pz - az;                                                                              \
        dx = (dx + 128) >> 8;                                                                      \
        dz = (dz + 128) >> 8;                                                                      \
        absX = dx;                                                                                 \
        absY = dy;                                                                                 \
        absZ = dz;                                                                                 \
        if (dx < 0)                                                                                \
            absX = -dx;                                                                            \
        if (dy < 0)                                                                                \
            absY = -dy;                                                                            \
        if (dz < 0)                                                                                \
            absZ = -dz;                                                                            \
    } while (0)

// confidence=low: steers toward the player and selects aligned attacks.
void sub_08187F44(struct ArmorChaseActor *actor)
{
    s32 playerX, actorX, playerZ, actorZ, deltaY, distance, absZ, direction;
    s32 absX;
    s32 absY;
    s32 deltaX;
    s32 deltaZ;

    u16 timer;
    u8 *decision;
    timer = actor->attackCooldown;
    if ((s16)timer > 0) {
        actor->attackCooldown = timer - 1;
        if (actor->fastMode)
            actor->attackCooldown = timer - 2;
    }
    timer = actor->collisionCooldown;
    if ((s16)timer > 0) {
        actor->collisionCooldown = timer - 1;
        if (actor->fastMode)
            actor->collisionCooldown = timer - 2;
        if ((s16)actor->collisionCooldown > 0)
            goto setCollision;
    }
    if (!(actor->flags & 1))
        Armor_ClearCollision(&actor->collision, 4);
    else {
    setCollision:
        Armor_SetCollision(&actor->collision);
    }
    if ((s16)--actor->modeTimer <= 0 && !(actor->flags & 1)) {
        actor->fastMode = !actor->fastMode;
        actor->modeTimer = 300;
    }
    if (actor->phase != 255 && actor->behavior != 19 && actor->behavior != 33 &&
        actor->behavior != 32 && actor->behavior != 34)
        return;
    ARMOR_PLAYER_DELTAS(actor, playerX, actorX, playerZ, actorZ, deltaX, deltaY, deltaZ, absX, absY,
                        absZ);
    if (actor->flags & 1) {
        decision = &actor->decision;
        switch (*decision) {
            case 0:
                if (absX + absZ <= 3) {
                    ActorController_SetBehavior(actor, 32);
                    *decision = 1;
                    actor->attackCooldown = 180;
                } else
                    ActorController_SetBehaviorIfFinished(actor, 19);
                break;
            case 1:
                if (actor->behavior == 3)
                    goto recover;
                distance = absX + absZ;
                if (distance > 4) {
                    ActorController_SetBehavior(actor, 19);
                    *decision = 0;
                    break;
                }
                direction = Math_Atan2(playerX - actorX, playerZ - actorZ);
                direction = (direction + 32) & 192;
                if (Armor_FacingValue(actor) == direction && distance <= 1)
                    goto attack;
                if ((s16)actor->attackCooldown <= 0) {
                    if (deltaX == 0) {
                        if (deltaZ < 0) {
                            if (actor->facing == 192)
                                goto alignHighZ;
                        } else if (actor->facing == 64)
                            goto alignHighZ;
                        goto idleHigh;
                    alignHighZ:
                        Armor_PickAxisAttack(actor);
                        break;
                    } else if (deltaZ == 0) {
                        if (deltaX < 0) {
                            if (actor->facing == 128)
                                goto alignHighX;
                        } else if (actor->facing == 0)
                            goto alignHighX;
                        goto idleHigh;
                    alignHighX:
                        Armor_PickAxisAttack(actor);
                        break;
                    }
                }
            idleHigh:
                ActorController_SetBehaviorIfFinished(actor, 32);
                break;
            case 2:
                if (actor->behavior == 15)
                    goto attack;
                ActorController_SetBehavior(actor, 32);
                *decision = 1;
                actor->attackCooldown = 180;
                break;
            case 3:
                if ((u8)(actor->behavior - 10) > 1 && actor->behavior != 13)
                    ActorController_SetBehavior(actor, 10);
                if ((s16)actor->attackCooldown > 0)
                    break;
                if (deltaX == 0) {
                    if (deltaZ < 0) {
                        if (actor->facing == 192)
                            goto strikeHighZ;
                    } else if (actor->facing == 64)
                        goto strikeHighZ;
                    goto resetHigh;
                strikeHighZ:
                    if (absY <= 127 && absZ <= 4) {
                        ActorController_SetBehavior(actor, 36);
                        actor->decision = 0;
                        break;
                    }
                    goto resetHigh;
                } else if (deltaZ == 0) {
                    if (deltaX < 0) {
                        if (actor->facing == 128)
                            goto strikeHighX;
                    } else if (actor->facing == 0)
                        goto strikeHighX;
                    goto resetHigh;
                strikeHighX:
                    if (absY <= 127 && absX <= 4) {
                        ActorController_SetBehavior(actor, 36);
                        actor->decision = 0;
                        break;
                    }
                    goto resetHigh;
                }
            resetHigh:
                ActorController_SetBehavior(actor, 19);
                actor->decision = 0;
                break;
        }
    } else {
        switch (actor->decision) {
            case 0:
                if (absX + absZ <= 3) {
                    if (actor->fastMode)
                        ActorController_SetBehavior(actor, 34);
                    else
                        ActorController_SetBehavior(actor, 32);
                    actor->decision = 1;
                    actor->attackCooldown = 180;
                } else if (Armor_IsSequenceIncomplete(actor->armorData))
                    ActorController_SetBehavior(actor, 8);
                else if (actor->fastMode)
                    ActorController_SetBehavior(actor, 33);
                else
                    ActorController_SetBehavior(actor, 19);
                break;
            case 1:
                if (actor->behavior == 3) {
                recover:
                    ActorController_SetBehavior(actor, 30);
                    break;
                }
                distance = absX + absZ;
                if (distance > 4) {
                    if (actor->fastMode)
                        ActorController_SetBehavior(actor, 33);
                    else
                        ActorController_SetBehavior(actor, 19);
                    actor->decision = 0;
                    break;
                }
                direction = Math_Atan2(playerX - actorX, playerZ - actorZ);
                direction = (direction + 32) & 192;
                if (Armor_FacingValue(actor) == direction && distance <= 1)
                    goto attack;
                if ((s16)actor->attackCooldown <= 0) {
                    if (deltaX == 0) {
                        if (deltaZ < 0) {
                            if (actor->facing == 192)
                                goto alignLowZ;
                        } else if (actor->facing == 64)
                            goto alignLowZ;
                        goto idleLow;
                    alignLowZ:
                        Armor_PickAxisAttack(actor);
                        break;
                    } else if (deltaZ == 0) {
                        if (deltaX < 0) {
                            if (actor->facing == 128)
                                goto alignLowX;
                        } else if (actor->facing == 0)
                            goto alignLowX;
                        goto idleLow;
                    alignLowX:
                        Armor_PickAxisAttack(actor);
                        break;
                    }
                }
            idleLow:
                if (actor->fastMode)
                    ActorController_SetBehavior(actor, 34);
                else
                    ActorController_SetBehavior(actor, 32);
                break;
            case 2:
                if (actor->behavior == 15) {
                attack:
                    ActorController_SetBehavior(actor, 3);
                    break;
                }
                if (actor->fastMode)
                    ActorController_SetBehavior(actor, 34);
                else
                    ActorController_SetBehavior(actor, 32);
                actor->decision = 1;
                actor->attackCooldown = 180;
                break;
        }
    }
}
