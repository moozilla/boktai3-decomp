#include "global.h"

#ifdef NONMATCHING
// fuzzy: 78.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: selects approach, attack and recovery behaviors along a shared X/Z axis.

// MATCHING_CONCESSION: register-pinning
// Remaining differences: branch layout, merged decision stores and register allocation.

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
extern u32 gRandomIndex;
void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);
// confidence=low: selects attacks when player and armor share a horizontal grid axis.
void Armor_UpdateAlignedAttackDecision(struct ArmorDecisionActor *p)
{
    register u32 mode asm("r6");
    s32 dx;
    s32 dy;
    s32 dz;
    s32 ax;
    s32 ay;
    register s32 az asm("r2");
    s16 *cooldown = &p->attackCooldown;
    u32 k1;
    if ((*cooldown) > 0)
        --(*cooldown);
    if ((p->phase != 255) && (p->behavior != 20))
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
    {
        u16 k3 = 1;
        mode = p->flags & k3;
    }
    asm("" : "+r"(mode));
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
                    if (dz < 0) {
                        if (p->facing != 192)
                            goto idle;
                    } else if (p->facing != 64)
                        goto idle;
                    if ((ay <= 127) && (az <= 1)) {
                        {
                            u8 k2 = 1;
                            p->decision = k2;
                        }
                        goto attack;
                    }
                    if (az > 3)
                        goto idle;
                } else if (dz == 0) {
                    if (dx < 0) {
                        if (p->facing != 128)
                            goto idle;
                    } else if (p->facing != 0)
                        goto idle;
                    if ((ay <= 127) && (ax <= 1)) {
                        p->decision = 1;
                        goto attack;
                    }
                    if (ax > 3)
                        goto idle;
                } else
                    goto idle;
                p->decision = 1;
                ActorController_SetBehavior(p, k1 = 10);
                break;

            case 1:
                if (p->behavior == 3)
                    goto recover;
                if ((p->behavior == 21) && (!((u16)(p->taskFlags & 4))))
                    *decision = 0;
                if (dx == 0) {
                    if (dz < 0) {
                        if (p->facing != 192)
                            goto reset;
                    } else if (p->facing != 64)
                        goto reset;
                    if ((ay <= 127) && (az <= 1))
                        goto attack;
                } else if (dz == 0) {
                    if (dx < 0) {
                        if (p->facing != 128)
                            goto reset;
                    } else if (p->facing != 0)
                        goto reset;
                    if ((ay <= 127) && (ax <= 1))
                        goto attack;
                } else
                    goto reset;
                ActorController_SetBehaviorIfFinished(p, 21);
                break;

            case 2:
                if ((((u8)(p->behavior - k1)) > 1) && (p->behavior != 13))
                    ActorController_SetBehavior(p, k1);
                if (p->attackCooldown > 0)
                    break;
                if (dx == 0)
                    p->decision = 1;
                else if (dz == 0)
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
                        if (az <= 1)
                            goto attack;
                    } else if (dz == 0) {
                        if (ax <= 1) {
                        attack:
                            ActorController_SetBehavior(p, 3);

                            break;
                        }
                    }
                }
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

#else
INCLUDE_ASM("asm/nonmatching", Armor_UpdateAlignedAttackDecision);
#endif
