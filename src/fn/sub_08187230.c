#include "global.h"
#include "game_state.h"
#include "scene_actor_components.h"
#include "contact_collider.h"

struct ArmorDecisionActor {
    u8 pad0[0x18];
    u16 taskFlags;
    u8 pad1A[0x3C - 0x1A];
    struct SceneActorBody body;
    u8 pad84[0x28C - 0x84];
    struct ContactCollider collision;
    u8 pad2E0[0xB32 - 0x2E0];
    u16 flags;
    u8 padb34[4];
    u8 type;
    u8 facing;
    u8 padb3a[2];
    u8 behavior, phase;
    u8 padB3E[0xB58 - 0xB3E];
    u8 sequence[0x1120 - 0xB58];
    u8 decision, fastMode;
    s16 attackCooldown, secondaryCooldown, collisionCooldown;
};

void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);

#define ARMOR_BRANCH_FACING_Z(actor, delta, aligned, unaligned) do { \
    if ((delta) < 0) { \
        if ((actor)->facing == 192) \
            goto aligned; \
    } else if ((actor)->facing == 64) \
        goto aligned; \
    goto unaligned; \
} while (0)

#define ARMOR_ABSOLUTE_COMPONENT(magnitude, value) do { \
    if ((value) < 0) \
        (magnitude) = -(value); \
} while (0)

#define ARMOR_BRANCH_OUTSIDE_GRID(magnitude, maximum, label) do { \
    if ((magnitude) > (maximum)) \
        goto label; \
} while (0)

static inline void Armor_MarkAttack(u8 *decision)
{
    *decision = 1;
}

/* Choose the Armor behavior from the player's direction and grid distance. */
void Armor_UpdateAxisAttackDecision(struct ArmorDecisionActor *p)
{
    u32 mode;
    s32 dx;
    s32 dy;
    s32 zDelta;
    s32 ax;
    s32 ay;
    s32 az;
    s16 *cooldown = &p->attackCooldown;

    if (*cooldown > 0)
        --*cooldown;
    if ((p->phase != 255) && (p->behavior != 20))
        return;
    dx = gGameState->positionX - p->body.position.x;
    dy = gGameState->positionY - p->body.position.y;
    zDelta = gGameState->positionZ - p->body.position.z;
    dx = (dx + 128) >> 8;
    zDelta = (zDelta + 128) >> 8;
    ax = dx;
    ay = dy;
    az = zDelta;
    if (dx < 0)
        ax = -dx;
    if (dy < 0)
        ay = -dy;
    ARMOR_ABSOLUTE_COMPONENT(az, zDelta);
    mode = p->flags & 1;
    if (mode) {
        u8 *decision = &p->decision;
        switch (*decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 12);
                    break;
                }
                if ((p->behavior != 20) || (!(p->taskFlags & 4)))
                    goto idle;
                if (dx == 0) {
                    ARMOR_BRANCH_FACING_Z(p, zDelta, aligned0, idle);
                aligned0:
                    if ((ay <= 127) && (az <= 1)) {
                        Armor_MarkAttack(&p->decision);
                        ActorController_SetBehavior(p, 3);
                        return;
                    }
                    ARMOR_BRANCH_OUTSIDE_GRID(az, 3, idle);
                    p->decision = 1;
                    ActorController_SetBehavior(p, 10);
                    return;
                } else if (zDelta == 0) {
                    s32 gridX = dx;
                    if (gridX < 0) {
                        if (p->facing == 128)
                            goto aligned1;
                    } else if (p->facing == 0)
                        goto aligned1;
                    goto idle;
                aligned1:
                    if ((ay <= 127) && (ax <= 1)) {
                        p->decision = 1;
                        goto attack;
                    }
                    if (ax > 3)
                        goto idle;
                } else
                    goto idle;
                p->decision = 1;
                ActorController_SetBehavior(p, 10);
                break;

            case 1:
                if (p->behavior == 3)
                    goto recover;
                if ((p->behavior == 21) && (!((u16)(p->taskFlags & 4))))
                    *decision = 0;
                if (dx == 0) {
                    ARMOR_BRANCH_FACING_Z(p, zDelta, aligned2, reset);
                aligned2:
                    if ((ay <= 127) && (az <= 1))
                        goto attack;
                } else if (zDelta == 0) {
                    if (dx < 0) {
                        if (p->facing == 128)
                            goto aligned3;
                    } else if (p->facing == 0)
                        goto aligned3;
                    goto reset;
                aligned3:
                    if ((ay <= 127) && (ax <= 1))
                        goto attack;
                } else
                    goto reset;
                ActorController_SetBehaviorIfFinished(p, 21);
                break;

            case 2:
                if ((u8)(p->behavior - 10) > 1 && p->behavior != 13)
                    ActorController_SetBehavior(p, 10);
                if (p->attackCooldown > 0)
                    break;
                if (dx == 0)
                    p->decision = 1;
                else if (zDelta == 0)
                    p->decision = 1;
                else {
                reset:
                    p->decision = 0;
                }
                break;

            case 16:
                ActorController_SetBehavior(p, 40);
                break;
        }
    } else {
        u8 *decision = &p->decision;
        switch (*decision) {
            case 0:
                if (p->behavior == 3) {
                recover:
                    ActorController_SetBehavior(p, 4);
                    break;
                }
                if (ay <= 127) {
                    if (dx == 0) {
                        if (az > 1)
                            goto far;
                    } else if (zDelta != 0 || ax > 1)
                        goto far;
                attack:
                    ActorController_SetBehavior(p, 3);
                    break;
                }
            far:
                if (++p->collisionCooldown > 300 &&
                    Armor_IsSequenceIncomplete(p->sequence)) {
                    ActorController_SetBehavior(p, 31);
                    p->collisionCooldown = 0;
                    break;
                }
            idle:
                ActorController_SetBehaviorIfFinished(p, 20);
                break;

            case 1:
                if (*cooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    *decision = mode;
                    p->secondaryCooldown = mode;
                    p->collisionCooldown = mode;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                break;

            case 2:
                ActorController_SetBehaviorIfFinished(p, 23);
                *decision = mode;
                p->secondaryCooldown = mode;
                p->collisionCooldown = mode;
                break;

            case 16:
                ActorController_SetBehavior(p, 40);
                break;
        }
    }
}
