#include "global.h"

#ifdef NONMATCHING
// fuzzy: 78.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: selects approach, attack and recovery behaviors along a shared X/Z axis.
// MATCHING_CONCESSION: register-pinning
// Remaining differences: branch sharing, decision stores and register allocation.
// confidence=low: attacks on a shared X/Z axis while choosing approach and recovery states.
struct ArmorAxisActor {
    u8 pad0[0x18];
    u16 taskFlags;
    u8 pad1[0x48 - 0x1a];
    s16 x, y, z;
    u8 pad2[0xb32 - 0x4e];
    u16 flags;
    u8 pad3[5];
    u8 facing;
    u8 pad4[2];
    u8 behavior, phase;
    u8 pad5[0x1120 - 0xb3e];
    u8 decision, fast;
    s16 attackTimer, secondaryTimer, collisionTimer;
};
struct ArmorAxisPlayer {
    u8 pad[0x30];
    s16 x, y, z;
};
extern struct ArmorAxisPlayer *gUnk_02000710;
void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
void Armor_UpdateAxisAttackDecision(struct ArmorAxisActor *p)
{
    s32 dx;
    s32 dy;
    s32 ax;
    s32 ay;
    s32 dz;
    register u32 mode asm("r6");
    register s32 az asm("r2");
    s16 *timer = &p->attackTimer;
    u32 k1;
    if ((*timer) > 0)
        --(*timer);
    if ((p->phase != 255) && (p->behavior != 20))
        return;
    dx = gUnk_02000710->x - p->x;
    dy = gUnk_02000710->y - p->y;
    dz = gUnk_02000710->z - p->z;
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
                if (p->behavior != 20)
                    goto idle;
                if (!(p->taskFlags & 4))
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
                } else {
                    if (dz != 0)
                        goto idle;
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
                }
                p->decision = 1;
                ActorController_SetBehavior(p, k1 = 10);
                break;

            case 1:
                if (p->behavior == 3)
                    goto recover;
                if ((p->behavior == 21) && (((u16)(p->taskFlags & 4)) == 0))
                    *decision = 0;
                if (dx == 0) {
                    if (dz < 0) {
                        if (p->facing != 192)
                            goto reset;
                    } else if (p->facing != 64)
                        goto reset;
                    if ((ay <= 127) && (az <= 1))
                        goto attack;
                } else {
                    if (dz != 0)
                        goto reset;
                    if (dx < 0) {
                        if (p->facing != 128)
                            goto reset;
                    } else if (p->facing != 0)
                        goto reset;
                    if ((ay <= 127) && (ax <= 1))
                        goto attack;
                }
                ActorController_SetBehaviorIfFinished(p, 21);
                break;

            case 2:
                if ((((u8)(p->behavior - k1)) > 1) && (p->behavior != 13))
                    ActorController_SetBehavior(p, k1);
                if (p->attackTimer > 0)
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
                if (((++p->collisionTimer) > 300) &&
                    Armor_IsSequenceIncomplete(((u8 *)p) + 0xb58)) {
                    ActorController_SetBehavior(p, 31);
                    p->collisionTimer = 0;
                    break;
                }
            idle:
                ActorController_SetBehaviorIfFinished(p, 20);

                break;

            case 1:
                if ((*timer) <= 0) {
                    ActorController_SetBehavior(p, 6);
                    *decision = mode;
                    p->secondaryTimer = mode;
                    p->collisionTimer = mode;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                break;

            case 2:
                ActorController_SetBehaviorIfFinished(p, 23);
                *decision = mode;
                p->secondaryTimer = mode;
                p->collisionTimer = mode;
                break;

            case 16:
                ActorController_SetBehavior(p, 40);
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Armor_UpdateAxisAttackDecision);
#endif
