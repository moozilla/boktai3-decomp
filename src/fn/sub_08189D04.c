#include "global.h"
#include "engine_globals.h"
/* Horizontal deltas use whole tiles; height stays in position units. */
#define ARMOR_ALIGNED_GRID(deltaX, deltaY, deltaZ, absX, absY, absZ, target, actor)                \
    do {                                                                                           \
        (deltaX) = (target)->x - (actor)->x;                                                       \
        (deltaY) = (target)->y - (actor)->y;                                                       \
        (deltaZ) = (target)->z - (actor)->z;                                                       \
        (deltaX) = ((deltaX) + 128) >> 8;                                                          \
        (deltaZ) = ((deltaZ) + 128) >> 8;                                                          \
        (absX) = (deltaX);                                                                         \
        (absY) = (deltaY);                                                                         \
        (absZ) = (deltaZ);                                                                         \
        if ((deltaX) < 0)                                                                          \
            (absX) = -(deltaX);                                                                    \
        if ((deltaY) < 0)                                                                          \
            (absY) = -(deltaY);                                                                    \
        if ((deltaZ) < 0)                                                                          \
            (absZ) = -(deltaZ);                                                                    \
    } while (0)

// MATCHING_CONCESSION: register-pinning
struct ArmorDecisionActor {
    u8 pad0[0x18];
    u16 taskFlags;
    u8 pad1a[0x48 - 0x1a];
    s16 x, y, z;
    u8 pad4e[0x28c - 0x4e];
    struct {
        u8 pad[6];
        s16 flags;
    } collision;
    u8 pad294[0xb32 - 0x294];
    u16 flags;
    u8 padb34[4];
    u8 type;
    u8 facing;
    u8 padb3a[2];
    u8 behavior, phase;
    u8 padb3e[0xb58 - 0xb3e];
    u8 sequence[0x1120 - 0xb58];
    u8 decision, fastMode;
    s16 attackCooldown, secondaryCooldown, sequenceTimer, modeTimer, timer112a;
    u16 savedB26, savedB28;
    s16 timer1130;
};
struct ArmorDecisionTarget {
    u8 pad[0x30];
    s16 x, y, z;
};
extern struct ArmorDecisionTarget *gGameState;
void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
static inline void Armor_ClearDecision(struct ArmorDecisionActor *actor, u8 *cachedDecision)
{
    u8 *address = &actor->decision;
    u32 zero = 0;
    // Keep the cached address live and the new address ahead of the zero value.
    asm("" : "+r"(zero) : "r"(address), "r"(cachedDecision));
    *address = zero;
}
// Tentative field meanings: aligned strike and recovery decisions.
void Armor_UpdateAlignedStrikeDecision(struct ArmorDecisionActor *actor)
{
    s32 deltaX;
    s32 deltaY;
    s32 deltaZ;
    s32 absX;
    s32 absY;
    s32 absZ;
    u32 flag;
    u8 *decisionPtr;
    s16 *cooldown = &actor->attackCooldown;
    if ((*cooldown) > 0)
        --(*cooldown);
    if ((((actor->phase != 255) && (actor->behavior != 20)) && (actor->behavior != 2)) &&
        (actor->behavior != 19))
        return;
    ARMOR_ALIGNED_GRID(deltaX, deltaY, deltaZ, absX, absY, absZ, gGameState, actor);
    flag = actor->flags & 1;
    if (flag) {
        decisionPtr = &actor->decision;
        switch (*decisionPtr) {
            case 0:
                if (actor->behavior == 3) {
                    ActorController_SetBehavior(actor, 4);
                    break;
                }
                if (absY <= 127) {
                    if (deltaX == 0) {
                        if (absZ > 1)
                            goto approach;
                        goto attack;
                    } else if (deltaZ == 0) {
                        if (absX <= 1) {
                        attack:
                            ActorController_SetBehavior(actor, 3);
                            return;
                        }
                    }
                }
            approach:
                if (((++actor->sequenceTimer) > 300) &&
                    Armor_IsSequenceIncomplete(actor->sequence)) {
                    ActorController_SetBehavior(actor, 31);
                    actor->sequenceTimer = 0;
                    break;
                }
                ActorController_SetBehaviorIfFinished(actor, 20);
                break;

            case 1:
                if (actor->attackCooldown <= 0) {
                    ActorController_SetBehavior(actor, 6);
                    {
                        u32 zero = 0;
                        *decisionPtr = zero;
                        actor->secondaryCooldown = zero;
                        actor->sequenceTimer = zero;
                    }
                } else
                    goto wait;
                break;

            case 2:
                ActorController_SetBehaviorIfFinished(actor, 23);
                {
                    u32 zero = 0;
                    *decisionPtr = zero;
                    actor->secondaryCooldown = zero;
                    actor->sequenceTimer = zero;
                }
                break;
        }

    } else {
        decisionPtr = &actor->decision;
        switch (*decisionPtr) {
            case 0:
                if (absY <= 127) {
                    if (deltaX == 0) {
                        if (absZ <= 1) {
                            ActorController_SetBehavior(actor, 3);
                            *decisionPtr = 1;
                            break;
                        }
                    } else if (deltaZ == 0) {
                        if (absX <= 1) {
                            ActorController_SetBehavior(actor, 3);
                            *decisionPtr = 1;
                            break;
                        }
                    }
                }
                ActorController_SetBehaviorIfFinished(actor, 19);
                break;

            case 1:
                if (absY <= 127) {
                    if (deltaX == 0) {
                        if ((deltaZ < 0) && (actor->facing == 192))
                            goto strike;
                        if (actor->facing == 64) {
                            ActorController_SetBehavior(actor, 22);
                            Armor_ClearDecision(actor, decisionPtr);
                            break;
                        }
                    } else if (deltaZ == 0) {
                        if ((deltaX < 0) && (actor->facing == 128)) {
                        strike:
                            ActorController_SetBehavior(actor, 22);
                            *decisionPtr = flag;
                            break;
                        }
                        if (actor->facing == 0) {
                            ActorController_SetBehavior(actor, 22);
                            actor->decision = 0;
                            break;
                        }
                    }
                }
                ActorController_SetBehavior(actor, 5);
                actor->decision = 2;
                actor->attackCooldown = 90;
                break;

            case 2:
                if (actor->attackCooldown <= 0) {
                    ActorController_SetBehavior(actor, 6);
                    *decisionPtr = flag;
                } else {
                wait:
                    ActorController_SetBehaviorIfFinished(actor, 5);
                }
                break;

            case 3:
                if (actor->behavior == 24)
                    *decisionPtr = flag;
                else
                    ActorController_SetBehavior(actor, 24);
                break;
        }
    }
    return;
}
