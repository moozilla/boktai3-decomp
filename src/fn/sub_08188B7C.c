#include "global.h"
#include "game_state.h"

struct ArmorPart {
    u8 pad[0x38];
    u32 flags;
};

struct ArmorChargeActor {
    u8 pad0[0x18];
    u16 taskFlags;
    u8 pad1a[0x48 - 0x1a];
    s16 x, y, z;
    u8 pad4e[0x190 - 0x4e];
    struct ArmorPart body;
    u8 pad1cc[0x1e4 - 0x1cc];
    struct ArmorPart attack;
    u8 pad220[0x274 - 0x220];
    u16 damage274;
    u8 pad276[0xb26 - 0x276];
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
};

struct ArmorChargeTarget {
    u8 pad[0x30];
    s16 x, y, z;
};

void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Math_Atan2(s32, s32);

static inline void Armor_ClearPart(struct ArmorPart *part, u32 bits)
{
    u32 mask = ~bits;
    part->flags &= mask;
}

/* Prepare a charge and clear the body and attack-part flags. */
static inline void Armor_BeginCharge(struct ArmorChargeActor *p)
{
    u16 damage;
    p->decision = 5;
    p->attackCooldown = 0x12C;
    p->savedB26 = p->field112C;
    damage = p->field112E;
    p->savedB28 = damage;
    p->damage274 = damage;
    Armor_ClearPart(&p->attack, 0x200004);
    Armor_ClearPart(&p->body, 0x200004);
}

/* Select a close strike or a more distant aligned attack. */
static inline void Armor_AimAtPlayer(struct ArmorChargeActor *p, s32 ax, s32 az,
                                     s32 allowDistantAttack)
{
    struct ArmorChargeTarget *tgt;
    s32 dx, dz, dir;

    tgt = (struct ArmorChargeTarget *)gGameState;
    dx = tgt->x - p->x;
    dz = tgt->z - p->z;
    dir = Math_Atan2(dx, dz);
    dir = (dir + 0x20) & 0xC0;
    if (p->facing == dir) {
        if (ax + az <= 2) {
            ActorController_SetBehavior(p, 3);
            return;
        }
        if (allowDistantAttack && ax + az > 4) {
            ActorController_SetBehavior(p, 9);
            return;
        }
    }
    if (p->secondaryCooldown <= 0)
        ActorController_SetBehaviorIfFinished(p, 0x1C);
    else
        ActorController_SetBehaviorIfFinished(p, 0x1B);
}

// confidence=low: manages aligned strikes, recovery and charge preparation.
void sub_08188B7C(struct ArmorChargeActor *p)
{
    s32 dx, dz, ax, az;
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
        struct ArmorChargeTarget *tgt = (struct ArmorChargeTarget *)gGameState;
        dx = tgt->x - p->x;
        dz = tgt->z - p->z;
    }
    dx = (dx + 128) >> 8;
    dz = (dz + 128) >> 8;
    ax = dx;
    az = dz;
    if (dx < 0)
        ax = -dx;
    if (dz < 0)
        az = -dz;

    if (p->flags & 1) {
        switch (p->decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (dx == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->taskFlags & 4))
                        goto waitAttack;
                    goto beginAttack;
                } else if (dz == 0) {
                    if (p->behavior == 2) {
                        if (p->taskFlags & 4)
                            goto beginAttack;
                    }
                    goto waitAttack;
                } else
                    ActorController_SetBehaviorIfFinished(p, 2);
                return;
            case 1:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 2);
                    p->decision = 0;
                    p->modeTimer = 0;
                    p->timer112a = 0;
                    return;
                } else {
                    s16 bh = p->behavior;
                    if (bh == 3) {
                        ActorController_SetBehavior(p, 10);
                        p->collisionCooldown = 120;
                        return;
                    }
                    if (bh == 15) {
                        ActorController_SetBehavior(p, 0xA);
                        p->collisionCooldown = 0x78;
                        return;
                    }
                    if ((u8)(bh - 10) <= 1 || bh == 0xD) {
                        if (p->collisionCooldown > 0)
                            return;
                    }
                    Armor_AimAtPlayer(p, ax, az, 0);
                    return;
                }
            case 2:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 0;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 3:
                if (p->secondaryCooldown <= 0)
                    Armor_BeginCharge(p);
                else {
                    if (p->flags & 2) {
                        ActorController_SetBehavior(p, 0xC);
                        return;
                    }
                    if (dx == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->taskFlags & 4))
                            goto waitWait;
                        goto beginWait;
                    } else if (dz == 0) {
                        if (p->behavior == 26) {
                            if (p->taskFlags & 4)
                                goto beginWait;
                        }
                        goto waitWait;
                    } else
                        ActorController_SetBehaviorIfFinished(p, 0x1A);
                }
                return;
            case 4:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 3;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 5:
                if (p->attackCooldown <= 0)
                    p->decision = 0;
                else
                    ActorController_SetBehavior(p, 0x1D);
                return;
        }
    } else {
        if (p->type == 0xD) {
            ActorController_SetBehavior(p, 0x29);
            return;
        }
        switch (p->decision) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (dx == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->taskFlags & 4))
                        goto waitAttack;
                    goto beginAttack;
                } else if (dz == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->taskFlags & 4))
                        goto waitAttack;
                beginAttack:
                    ActorController_SetBehavior(p, 24);
                    return;
                waitAttack:
                    ActorController_SetBehaviorIfFinished(p, 2);
                    return;
                } else
                    ActorController_SetBehaviorIfFinished(p, 2);
                return;
            case 1:
                if (p->attackCooldown <= 0) {
                    p->decision = 0;
                    p->modeTimer = 0;
                    p->timer112a = 0;
                    return;
                } else {
                    if (p->behavior == 3) {
                        ActorController_SetBehavior(p, 0x1E);
                        return;
                    }
                    Armor_AimAtPlayer(p, ax, az, 1);
                    return;
                }
            case 2:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 0;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 3:
                if (p->secondaryCooldown <= 0)
                    Armor_BeginCharge(p);
                else {
                    if (dx == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->taskFlags & 4))
                            goto waitWait;
                        goto beginWait;
                    } else if (dz == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->taskFlags & 4))
                            goto waitWait;
                    beginWait:
                        ActorController_SetBehavior(p, 25);
                        return;
                    waitWait:
                        ActorController_SetBehaviorIfFinished(p, 26);
                        return;
                    } else
                        ActorController_SetBehaviorIfFinished(p, 0x1A);
                }
                return;
            case 4:
                if (p->attackCooldown <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->decision = 3;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 5:
                if (p->attackCooldown <= 0)
                    p->decision = 0;
                else
                    ActorController_SetBehavior(p, 0x1D);
                return;
        }
    }
}
