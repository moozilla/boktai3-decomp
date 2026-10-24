#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: shared strike tails, zero/address scheduling and delta register allocation.
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
extern struct ArmorDecisionTarget *gUnk_02000710;
extern u32 gUnk_03005308;
void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);
// confidence=low: coordinates approach, aligned strike and recovery decisions.
void Armor_UpdateAlignedStrikeDecision(struct ArmorDecisionActor *p)
{
    register s32 dx asm("r5");
    u32 dy;
    register s32 dz asm("r1");
    register s32 ax asm("r7");
    s32 ay;
    s32 az;
    register u32 mode asm("r8");
    u8 *decision;
    s16 *cooldown = &p->attackCooldown;
    typeof(dz) c1;
    if ((*cooldown) > 0)
        --(*cooldown);
    if ((((p->phase != 255) && (p->behavior != 20)) && (p->behavior != 2)) && (p->behavior != 19))
        return;
    dx = gUnk_02000710->x - p->x;
    dy = gUnk_02000710->y - p->y;
    dz = gUnk_02000710->z - p->z;
    c1 = dz;
    dx = (dx + 128) >> 8;
    dz = (c1 + 128) >> 8;
    ax = dx;
    ay = dy;
    az = dz;
    if (dx < 0)
        ax = -dx;
    if (dy < 0)
        ay = -dy;
    if (dz < 0)
        az = -dz;
    asm("" : "+r"(dx), "+r"(dy), "+r"(dz), "+r"(ax));
    mode = p->flags & 1;
    asm("" : "+r"(mode));
    if (mode) {
        decision = &p->decision;
        switch (*decision) {
            case 0:
                if (p->behavior == 3) {
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

                            return;
                        }
                    }
                }
                if (((++p->collisionCooldown) > 300) &&
                    Armor_IsSequenceIncomplete(((u8 *)p) + 0xb58)) {
                    ActorController_SetBehavior(p, 31);
                    p->collisionCooldown = 0;
                    break;
                }
                ActorController_SetBehaviorIfFinished(p, 20);
                break;

            case 1:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    {
                        u8 k1 = 0;
                        *decision = k1;
                    }
                    p->secondaryCooldown = 0;
                    p->collisionCooldown = 0;
                } else
                    goto wait;
                break;

            case 2:
                ActorController_SetBehaviorIfFinished(p, 23);
                *decision = 0;
                {
                    typeof(p->secondaryCooldown) *q1 = &p->secondaryCooldown;
                    *q1 = 0;
                }
                p->collisionCooldown = 0;
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
                        if ((dz < 0) && (p->facing == 192))
                            goto strike;
                        if (p->facing == 64) {
                            ActorController_SetBehavior(p, 22);
                            p->decision = 0;
                            break;
                        }
                    } else if (dz == 0) {
                        if ((dx < 0) && (p->facing == 128)) {
                        strike:
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

#else
INCLUDE_ASM("asm/nonmatching", Armor_UpdateAlignedStrikeDecision);
#endif
