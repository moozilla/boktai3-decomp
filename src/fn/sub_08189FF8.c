#include "global.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.

// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.
struct S08189FF8Actor {
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
};
struct S08189FF8Target {
    u8 pad[0x30];
    s16 x, y, z;
};
void ActorController_SetBehavior(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);

static inline u16 S08189FF8_Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}

#ifdef NONMATCHING
// fuzzy: 53.4%
// scope shape 177 (differences once low registers are set aside)
// confidence=low: armor decision step: counts down the cooldown and mode timers, then picks the next behavior from the target offset, facing and phase.
void sub_08189FF8(struct S08189FF8Actor *p)
{
    struct S08189FF8Target *tg;
    s32 rawX;
    s32 rawZ;
    s32 dx;
    s32 dz;
    s32 adx;
    s32 adz;
    s32 dist;
    s32 dir;
    s16 t;
    s32 ok;

    t = p->attackCooldown;
    if (t > 0) {
        p->attackCooldown = t - 1;
        if (p->fastMode)
            p->attackCooldown = t - 2;
    }
    t = p->collisionCooldown;
    if (t > 0) {
        p->collisionCooldown = t - 1;
        if (p->fastMode)
            p->collisionCooldown = t - 2;
    }
    if (p->collisionCooldown > 0 || !(p->flags & 1)) {
        p->collision.flags |= 4;
    } else {
        p->collision.flags &= ~4;
    }

    p->modeTimer--;
    if (p->modeTimer <= 0) {
        if (p->flags & 1) {
            p->fastMode = !p->fastMode;
            p->modeTimer = 300;
        } else {
            p->modeTimer = 0;
        }
    }
    if (!(p->flags & 1))
        p->fastMode = 0;

    if (p->phase != 0xff) {
        u8 b = p->behavior;
        if (b != 0x13 && b != 0x21 && b != 0x20 && b != 0x22 && b != 2 && b != 0x25)
            return;
    }

    tg = (struct S08189FF8Target *)gGameState;
    rawX = tg->x - p->x;
    rawZ = tg->z - p->z;
    dx = (rawX + 0x80) >> 8;
    dz = (rawZ + 0x80) >> 8;
    adx = dx;
    adz = dz;
    if (adx < 0)
        adx = -adx;
    if (adz < 0)
        adz = -adz;
    dist = adx + adz;

    if (p->flags & 1) {
        if (p->decision == 0) {
            if (dist <= 3) {
                if (p->fastMode)
                    ActorController_SetBehavior(p, 0x22);
                else
                    ActorController_SetBehavior(p, 0x20);
                p->decision = 1;
                p->attackCooldown = 180;
            } else if (Armor_IsSequenceIncomplete((u8 *)p + 0xB58)) {
                ActorController_SetBehavior(p, 8);
            } else {
                if (p->fastMode)
                    ActorController_SetBehavior(p, 0x21);
                else
                    ActorController_SetBehavior(p, 0x13);
            }
        } else if (p->decision == 1) {
            if (p->behavior == 3) {
                ActorController_SetBehavior(p, 0x1e);
                return;
            }
            if (dist > 4) {
                if (p->fastMode)
                    ActorController_SetBehavior(p, 0x21);
                else
                    ActorController_SetBehavior(p, 0x13);
                p->decision = 0;
                return;
            }
            dir = (Math_Atan2(rawX, rawZ) + 0x20) & 0xC0;
            if (p->facing == dir && dist <= 1) {
                ActorController_SetBehavior(p, 3);
                return;
            }
            ok = 0;
            if (p->attackCooldown <= 0) {
                if (dx == 0 && dz < 0)
                    ok = (p->facing == 0xC0);
                else if (dx == 0 && dz >= 0)
                    ok = (p->facing == 0x40);
                else if (dz == 0 && dx < 0)
                    ok = (p->facing == 0x80);
                else if (dz == 0 && dx >= 0)
                    ok = (p->facing == 0);
            }
            if (ok) {
                if (S08189FF8_Rand() & 4)
                    ActorController_SetBehavior(p, 0x24);
                else
                    ActorController_SetBehavior(p, 0x18);
                return;
            }
            if (p->fastMode)
                ActorController_SetBehavior(p, 0x22);
            else
                ActorController_SetBehavior(p, 0x20);
        } else if (p->decision == 2) {
            if (p->behavior == 0xf) {
                ActorController_SetBehavior(p, 3);
                return;
            }
            if (p->fastMode)
                ActorController_SetBehavior(p, 0x22);
            else
                ActorController_SetBehavior(p, 0x20);
            p->decision = 1;
            p->attackCooldown = 180;
        }
    } else {
        if (p->decision == 0) {
            if (dist <= 4) {
                ActorController_SetBehavior(p, 0x25);
                p->decision = 1;
                p->attackCooldown = 360;
            } else {
                ActorController_SetBehavior(p, 2);
            }
        } else if (p->decision == 1) {
            if (p->behavior == 3) {
                ActorController_SetBehavior(p, 0x1e);
                return;
            }
            dir = (Math_Atan2(rawX, rawZ) + 0x20) & 0xC0;
            if (p->facing == dir) {
                if (dist > 5) {
                    ActorController_SetBehavior(p, 7);
                    p->decision = 0;
                    return;
                }
                if (dist <= 1) {
                    ActorController_SetBehavior(p, 3);
                    return;
                }
            }
            if (p->attackCooldown > 0) {
                ActorController_SetBehavior(p, 0x25);
            } else if (dx == 0 || dz == 0) {
                ActorController_SetBehavior(p, 0x24);
                p->attackCooldown = 360;
            } else {
                ActorController_SetBehavior(p, 0x25);
            }
        } else if (p->decision == 2) {
            ActorController_SetBehavior(p, 3);
            p->decision = 1;
            p->attackCooldown = 360;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08189FF8);
#endif
