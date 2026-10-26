#include "global.h"
#include "game_state.h"

/* confidence=low: behavior 2 with the task-flag 4 set goes to 0x18, otherwise wait for 2 */

/* confidence=low: behavior 0x1A with the task-flag 4 set goes to 0x19, otherwise wait for 0x1A */

/* confidence=low: start a charge: reset the timers and clear two actor-part flags */

/* confidence=low: face the target along the grid axis; timed adds the wait-for-sequence check */

/* confidence=low: behavior 2 with the task-flag 4 set goes to 0x18, otherwise wait for 2 */

/* confidence=low: behavior 0x1A with the task-flag 4 set goes to 0x19, otherwise wait for 0x1A */

/* confidence=low: start a charge: reset the timers and clear two actor-part flags */

/* confidence=low: face the target along the grid axis; timed adds the wait-for-sequence check */

struct Sub081885BCActor {
    u8 pad0[0x18];
    u16 taskFlags;
    u8 pad1a[0x48 - 0x1a];
    s16 x, y, z;
    u8 pad4e[0xB26 - 0x4e];
    u16 savedB26;
    u16 savedB28;
    u8 padB2A[0xB32 - 0xB2A];
    u16 flags;
    u8 padB34[4];
    u8 type;
    u8 facing;
    u8 padB3A[2];
    u8 behavior, phase;
    u8 padB3E[0x1120 - 0xB3E];
    u8 decision, fastMode;
    s16 attackCooldown;
    s16 secondaryCooldown;
    s16 collisionCooldown;
    s16 modeTimer;
    s16 timer112a;
    u16 field112C;
    u16 field112E;
    s16 timer1130;
};

struct Sub081885BCTarget {
    u8 pad[0x30];
    s16 x, y, z;
};

void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);

/* confidence=low: behavior 2 with the task-flag 4 set goes to 0x18, otherwise wait for 2 */
static inline void sub_081885BC_ChangeIfAttacking(struct Sub081885BCActor *p)
{
    if (p->behavior == 2 && (p->taskFlags & 4))
        ActorController_SetBehavior(p, 0x18);
    else
        ActorController_SetBehaviorIfFinished(p, 2);
}

/* confidence=low: behavior 0x1A with the task-flag 4 set goes to 0x19, otherwise wait for 0x1A */
static inline void sub_081885BC_ChangeIfWaiting(struct Sub081885BCActor *p)
{
    if (p->behavior == 0x1A && (p->taskFlags & 4))
        ActorController_SetBehavior(p, 0x19);
    else
        ActorController_SetBehaviorIfFinished(p, 0x1A);
}

/* confidence=low: start a charge: reset the timers and clear two actor-part flags */
static inline void sub_081885BC_BeginCharge(struct Sub081885BCActor *p)
{
    p->decision = 5;
    p->attackCooldown = 0x12C;
    p->savedB26 = p->field112C;
    p->savedB28 = p->field112E;
    *(u16 *)((u8 *)p + 0x274) = p->field112E;
    *(u32 *)((u8 *)p + 0x21C) &= 0xFFDFFFFB;
    *(u32 *)((u8 *)p + 0x1C8) &= 0xFFDFFFFB;
}

/* confidence=low: face the target along the grid axis; timed adds the wait-for-sequence check */
static inline void sub_081885BC_Aim(struct Sub081885BCActor *p, s32 ax, s32 az, s32 timed)
{
    struct Sub081885BCTarget *tgt;
    s32 dx, dz, dir;

    tgt = (struct Sub081885BCTarget *)gGameState;
    dx = tgt->x - p->x;
    dz = tgt->z - p->z;
    dir = (Math_Atan2(dx, dz) + 0x20) & 0xC0;
    if (p->facing == dir && ax + az <= 2) {
        ActorController_SetBehavior(p, 3);
        return;
    }
    if (timed) {
        if (p->timer1130 > 0x12C && Armor_IsSequenceIncomplete((u8 *)p + 0xB58)) {
            ActorController_SetBehavior(p, 0x1F);
            p->timer1130 = 0;
            return;
        }
    }
    if (p->secondaryCooldown > 0)
        ActorController_SetBehaviorIfFinished(p, 0x1B);
    else
        ActorController_SetBehaviorIfFinished(p, 0x1C);
}

#ifdef NONMATCHING
// fuzzy: 33.6%
// scope shape 287 (differences once low registers are set aside)
void sub_081885BC(void *arg0)
{
    struct Sub081885BCActor *p;
    s32 dx, dz, tx, tz, ax, az;

    p = arg0;
    if (p->attackCooldown > 0)
        p->attackCooldown--;
    if (p->secondaryCooldown > 0)
        p->secondaryCooldown--;
    if (p->collisionCooldown > 0)
        p->collisionCooldown--;
    if (p->phase != 0xFF && p->behavior != 2 && p->behavior != 0x1A && p->behavior != 0x1B &&
        p->behavior != 0x1C)
        return;
    {
        struct Sub081885BCTarget *tgt = (struct Sub081885BCTarget *)gGameState;
        dx = tgt->x - p->x;
        dz = tgt->z - p->z;
    }
    tx = (dx + 0x80) >> 8;
    tz = (dz + 0x80) >> 8;
    ax = tx < 0 ? -tx : tx;
    az = tz < 0 ? -tz : tz;

    if (p->flags & 1) {
        if (p->decision > 5)
            return;
        switch (p->decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (tx == 0 || tz == 0)
                    sub_081885BC_ChangeIfAttacking(p);
                else
                    ActorController_SetBehaviorIfFinished(p, 2);
                return;
            case 1:
                if (p->attackCooldown > 0) {
                    u8 bh = p->behavior;
                    if (bh == 3 || bh == 0xF) {
                        ActorController_SetBehavior(p, 0xA);
                        p->collisionCooldown = 0x78;
                        return;
                    }
                    if ((u8)(bh - 10) <= 1 || bh == 0xD) {
                        if (p->collisionCooldown > 0)
                            return;
                    }
                    sub_081885BC_Aim(p, ax, az, 0);
                    return;
                }
                ActorController_SetBehavior(p, 2);
                p->decision = 0;
                p->modeTimer = 0;
                p->timer112a = 0;
                return;
            case 2:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 5);
                else {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 0;
                }
                return;
            case 3:
                if (p->secondaryCooldown > 0) {
                    if (p->flags & 2) {
                        ActorController_SetBehavior(p, 0xC);
                        return;
                    }
                    if (tx == 0 || tz == 0)
                        sub_081885BC_ChangeIfWaiting(p);
                    else
                        ActorController_SetBehaviorIfFinished(p, 0x1A);
                } else
                    sub_081885BC_BeginCharge(p);
                return;
            case 4:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 5);
                else {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 3;
                }
                return;
            default:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 0x1D);
                else
                    p->decision = 0;
                return;
        }
    } else {
        if (p->type == 0xC) {
            ActorController_SetBehavior(p, 0x29);
            return;
        }
        if (p->decision > 5)
            return;
        switch (p->decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (tx == 0 || tz == 0)
                    sub_081885BC_ChangeIfAttacking(p);
                else
                    ActorController_SetBehaviorIfFinished(p, 2);
                return;
            case 1:
                if (p->attackCooldown > 0) {
                    if (p->behavior == 3) {
                        ActorController_SetBehavior(p, 0x1E);
                        return;
                    }
                    sub_081885BC_Aim(p, ax, az, 1);
                    return;
                }
                p->decision = 0;
                p->modeTimer = 0;
                p->timer112a = 0;
                return;
            case 2:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 5);
                else {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 0;
                }
                return;
            case 3:
                if (p->secondaryCooldown > 0) {
                    if (tx == 0 || tz == 0)
                        sub_081885BC_ChangeIfWaiting(p);
                    else
                        ActorController_SetBehaviorIfFinished(p, 0x1A);
                } else
                    sub_081885BC_BeginCharge(p);
                return;
            case 4:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 5);
                else {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 3;
                }
                return;
            default:
                if (p->attackCooldown > 0)
                    ActorController_SetBehaviorIfFinished(p, 0x1D);
                else
                    p->decision = 0;
                return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081885BC);
#endif
