#include "global.h"
#include "engine_globals.h"
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
s32 Math_Atan2(s32, s32);
// confidence=low: coordinates approach, aligned strike and recovery decisions.
void Armor_SelectTimedAttack(struct ArmorDecisionActor *p)
{
    s32 dx, dy, dz, ax, ay, az;
    u32 mode;
    u8 *decision;
    s16 *cooldown = &p->attackCooldown;
    if (*cooldown > 0)
        --*cooldown;
    if (p->phase != 255 && p->behavior != 2 && p->behavior != 19)
        return;
    dx = gGameState->x - p->x;
    dy = gGameState->y - p->y;
    dz = gGameState->z - p->z;
    dx = (dx + 128) >> 8;
    dz = (dz + 128) >> 8;
    ax = dx;
    ay = dy;
    az = dz;
    if (dx < 0)
        ax = -dx;
    if (dy < 0)
        ay = -dy;
    if (dz < 0)
        az = -dz;
    mode = p->flags & 1;
    if (mode) {
        decision = &p->decision;
        switch (*decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 12);
                    break;
                }
                if (ay > 127)
                    goto far;
                if (dx == 0) {
                    if (az > 1)
                        goto far;
                } else {
                    if (dz != 0 || ax > 1)
                        goto far;
                }
                ActorController_SetBehavior(p, 4);
                break;
            far:
                ActorController_SetBehaviorIfFinished(p, 2);
                break;

            case 1:
                if (p->attackCooldown <= 0) {
                    *decision = 0;
                    break;
                }
                if (p->behavior == 21 && !(u16)(p->taskFlags & 4)) {
                    *decision = 0;
                    break;
                }
                if (dx == 0) {
                    if (dz < 0) {
                        if (p->facing == 192)
                            goto aligned;
                    } else if (p->facing == 64)
                        goto aligned;
                    p->decision = 0;
                } else if (dz == 0) {
                    if (dx < 0) {
                        if (p->facing == 128)
                            goto aligned;
                    } else if (p->facing == 0)
                        goto aligned;
                    p->decision = 0;
                } else
                    p->decision = 0;
                break;
            aligned:
                if ((u8)(p->behavior - 10) > 1 && p->behavior != 13 && p->behavior != 21) {
                    ActorController_SetBehavior(p, 10);
                    break;
                }
                if (p->behavior == 11 && (ax > 1 || az > 1))
                    ActorController_SetBehavior(p, 21);
                break;
            case 2:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    *decision = 0;
                } else
                    goto wait;
                break;
        }
    } else {
        decision = &p->decision;
        switch (*decision) {
            case 0:
                if (ay <= 127) {
                    if (dx == 0) {
                        if (az <= 1) {
                            ActorController_SetBehavior(p, 3);
                            *decision = 1;
                            break;
                        }
                    } else if (dz == 0) {
                        if (ax <= 1) {
                            ActorController_SetBehavior(p, 3);
                            *decision = 1;
                            break;
                        }
                    }
                }
                ActorController_SetBehaviorIfFinished(p, 19);
                break;
            case 1:
                if (ay <= 127) {
                    if (dx == 0) {
                        if (dz < 0 && p->facing == 192) {
                            ActorController_SetBehavior(p, 22);
                            *decision = mode;
                            break;
                        }
                        if (p->facing == 64) {
                            ActorController_SetBehavior(p, 22);
                            p->decision = 0;
                            break;
                        }
                    } else if (dz == 0) {
                        if (dx < 0 && p->facing == 128) {
                            ActorController_SetBehavior(p, 22);
                            *decision = mode;
                            break;
                        }
                        if (p->facing == 0) {
                            ActorController_SetBehavior(p, 22);
                            p->decision = 0;
                            break;
                        }
                    }
                }
                ActorController_SetBehavior(p, 5);
                p->decision = 2;
                p->attackCooldown = 90;
                break;
            case 2:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    *decision = mode;
                } else {
                wait:
                    ActorController_SetBehaviorIfFinished(p, 5);
                }
                break;
            case 3:
                if (p->behavior == 24)
                    *decision = mode;
                else
                    ActorController_SetBehavior(p, 24);
                break;
        }
    }
    return;
}
