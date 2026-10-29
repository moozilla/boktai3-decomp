#include "global.h"
#include "engine_globals.h"

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
    u8 padb3e[0x1120 - 0xb3e];
    u8 decision, fastMode;
    s16 attackCooldown, secondaryCooldown, collisionCooldown, modeTimer, timer112a;
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
// confidence=low: selects attacks when player and armor share a horizontal grid axis.
void Armor_UpdateAlignedAttackDecision(struct ArmorDecisionActor *p)
{
    u32 mode;
    s32 dx;
    s32 dy;
    s32 zDelta;
    register s32 gridZ asm("r5");
    s32 ax;
    s32 ay;
    register s32 az asm("r2");
    s16 *cooldown = &p->attackCooldown;
    if ((*cooldown) > 0)
        --(*cooldown);
    if ((p->phase != 255) && (p->behavior != 20))
        return;
    dx = gGameState->x - p->x;
    dy = gGameState->y - p->y;
    zDelta = gGameState->z - p->z;
    dx = (dx + 128) >> 8;
    zDelta = (zDelta + 128) >> 8;
    ax = dx;
    ay = dy;
    az = zDelta;
    if (dx < 0)
        ax = -dx;
    if (dy < 0)
        ay = -dy;
    if (zDelta < 0)
        az = -zDelta;
    gridZ = zDelta;
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
                    if (gridZ < 0) {
                        if (p->facing == 192)
                            goto aligned0;
                    } else if (p->facing == 64)
                        goto aligned0;
                    goto idle;
                aligned0:
                    if ((ay <= 127) && (az <= 1)) {
                        p->decision = 1;
                        // Preserve this state store before the shared attack tail.
                        asm volatile("" : : "m"(p->decision));
                        goto attack;
                    }
                    if (az > 3)
                        goto idle;
                } else if (gridZ == 0) {
                    register s32 gridX asm("r5") = dx;
                    asm("" : : "r"(gridX));
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
                    if (gridZ < 0) {
                        if (p->facing == 192)
                            goto aligned2;
                    } else if (p->facing == 64)
                        goto aligned2;
                    goto reset;
                aligned2:
                    if ((ay <= 127) && (az <= 1))
                        goto attack;
                } else if (gridZ == 0) {
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
                if ((((u8)(p->behavior - 10)) > 1) && (p->behavior != 13))
                    ActorController_SetBehavior(p, 10);
                if (p->attackCooldown > 0)
                    break;
                if (dx == 0)
                    p->decision = 1;
                else if (gridZ == 0)
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
                    } else if (gridZ != 0 || ax > 1)
                        goto far;
                attack:
                    ActorController_SetBehavior(p, 3);
                    break;
                }
            far:
                if (((++p->collisionCooldown) > 300) &&
                    Armor_IsSequenceIncomplete(((u8 *)p) + 0xb58)) {
                    ActorController_SetBehavior(p, 31);
                    p->collisionCooldown = 0;
                    break;
                }
            idle:
                ActorController_SetBehaviorIfFinished(p, 20);

                break;

            case 1:
                if ((*cooldown) <= 0) {
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
