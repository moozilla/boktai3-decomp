#include "global.h"
#include "scene/collision/contact_collider.h"
#include "system/game_state/game_state.h"
#include "monsters/armor/scene_combat_colliders.h"


void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);

/* Prepare a charge and clear the body and attack-part flags. */
static inline void Armor_BeginCharge(struct SceneCombatActor *p)
{
    u16 damage;
    p->state.common.phase = 5;
    p->state.common.counter = 0x12C;
    p->attack = p->state.common.savedAttack;
    damage = p->state.common.savedDefense;
    p->defense = damage;
    p->colliderGroup.colliders[2].unknown3C = damage;
    ContactCollider_ClearReactionFlags(&p->colliderGroup.colliders[1], 0x200004);
    ContactCollider_ClearReactionFlags(&p->colliderGroup.colliders[0], 0x200004);
}

/* Select an aligned strike, then handle the timed fallback. */
static inline void Armor_AimAtPlayer(struct SceneCombatActor *p, s32 ax, s32 az, s32 timed)
{
    struct GameState *tgt;
    s32 dx, dz, dir;

    tgt = gGameState;
    dx = tgt->positionX - p->navigation.referencePosition.x;
    dz = tgt->positionZ - p->navigation.referencePosition.z;
    dir = Math_Atan2(dx, dz);
    dir = (dir + 0x20) & 0xC0;
    if (p->heading == dir && ax + az <= 2) {
        ActorController_SetBehavior(p, 3);
        return;
    }
    if (timed) {
        if ((s16)p->state.value10 > 0x12C && Armor_IsSequenceIncomplete(&p->parts)) {
            ActorController_SetBehavior(p, 0x1F);
            p->state.value10 = 0;
            return;
        }
    }
    if ((s16)p->state.common.timer <= 0)
        ActorController_SetBehaviorIfFinished(p, 0x1C);
    else
        ActorController_SetBehaviorIfFinished(p, 0x1B);
}

// confidence=low: manages aligned strikes, recovery and charge preparation.
void sub_081885BC(struct SceneCombatActor *p)
{
    s32 dx, dz, ax, az;
    if ((s16)p->state.common.counter > 0)
        p->state.common.counter--;
    if ((s16)p->state.common.timer > 0)
        p->state.common.timer--;
    if ((s16)p->state.common.value6 > 0)
        p->state.common.value6--;
    if (p->behaviorPhase != 0xFF && p->behavior != 2 && p->behavior != 0x1A && p->behavior != 0x1B &&
        p->behavior != 0x1C)
        return;
    {
        struct GameState *tgt = gGameState;
        dx = tgt->positionX - p->navigation.referencePosition.x;
        dz = tgt->positionZ - p->navigation.referencePosition.z;
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
        switch (p->state.common.phase) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (dx == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->navigation.probe[0] & 4))
                        goto waitAttack;
                    goto beginAttack;
                } else if (dz == 0) {
                    if (p->behavior == 2) {
                        if (p->navigation.probe[0] & 4)
                            goto beginAttack;
                    }
                    goto waitAttack;
                } else
                    ActorController_SetBehaviorIfFinished(p, 2);
                return;
            case 1:
                if ((s16)p->state.common.counter <= 0) {
                    ActorController_SetBehavior(p, 2);
                    p->state.common.phase = 0;
                    p->state.common.value8 = 0;
                    p->state.common.valueA = 0;
                    return;
                } else {
                    s16 bh = p->behavior;
                    if (bh == 3) {
                        ActorController_SetBehavior(p, 10);
                        p->state.common.value6 = 120;
                        return;
                    }
                    if (bh == 15) {
                        ActorController_SetBehavior(p, 0xA);
                        p->state.common.value6 = 0x78;
                        return;
                    }
                    if ((u8)(bh - 10) <= 1 || bh == 0xD) {
                        if ((s16)p->state.common.value6 > 0)
                            return;
                    }
                    Armor_AimAtPlayer(p, ax, az, 0);
                    return;
                }
            case 2:
                if ((s16)p->state.common.counter <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->state.common.phase = 0;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 3:
                if ((s16)p->state.common.timer <= 0)
                    Armor_BeginCharge(p);
                else {
                    if (p->flags & 2) {
                        ActorController_SetBehavior(p, 0xC);
                        return;
                    }
                    if (dx == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->navigation.probe[0] & 4))
                            goto waitWait;
                        goto beginWait;
                    } else if (dz == 0) {
                        if (p->behavior == 26) {
                            if (p->navigation.probe[0] & 4)
                                goto beginWait;
                        }
                        goto waitWait;
                    } else
                        ActorController_SetBehaviorIfFinished(p, 0x1A);
                }
                return;
            case 4:
                if ((s16)p->state.common.counter <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->state.common.phase = 3;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 5:
                if ((s16)p->state.common.counter <= 0)
                    p->state.common.phase = 0;
                else
                    ActorController_SetBehaviorIfFinished(p, 0x1D);
                return;
        }
    } else {
        if (p->kind == 0xC) {
            ActorController_SetBehavior(p, 0x29);
            return;
        }
        switch (p->state.common.phase) {
            case 0:
                if (p->flags & 2) {
                    ActorController_SetBehavior(p, 0xC);
                    return;
                }
                if (dx == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->navigation.probe[0] & 4))
                        goto waitAttack;
                    goto beginAttack;
                } else if (dz == 0) {
                    if (p->behavior != 2)
                        goto waitAttack;
                    if (!(p->navigation.probe[0] & 4))
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
                if ((s16)p->state.common.counter <= 0) {
                    p->state.common.phase = 0;
                    p->state.common.value8 = 0;
                    p->state.common.valueA = 0;
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
                if ((s16)p->state.common.counter <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->state.common.phase = 0;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 3:
                if ((s16)p->state.common.timer <= 0)
                    Armor_BeginCharge(p);
                else {
                    if (dx == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->navigation.probe[0] & 4))
                            goto waitWait;
                        goto beginWait;
                    } else if (dz == 0) {
                        if (p->behavior != 26)
                            goto waitWait;
                        if (!(p->navigation.probe[0] & 4))
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
                if ((s16)p->state.common.counter <= 0) {
                    ActorController_SetBehavior(p, 6);
                    p->state.common.phase = 3;
                } else
                    ActorController_SetBehaviorIfFinished(p, 5);
                return;
            case 5:
                if ((s16)p->state.common.counter <= 0)
                    p->state.common.phase = 0;
                else
                    ActorController_SetBehaviorIfFinished(p, 0x1D);
                return;
        }
    }
}
