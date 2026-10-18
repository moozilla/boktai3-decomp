#include "global.h"
struct ArmorActor {
    u8 pad0[0x238];
    u8 damageDisplay[0x244 - 0x238];
    s32 damagePosition[4];
    u8 pad1[0xb18 - 0x254];
    u32 actorId;
    s16 health;
    s16 maxHealth;
    u8 pad2[0xb32 - 0xb20];
    u16 flags;
    u8 pad3[0xb39 - 0xb34];
    u8 direction;
    u8 pad4[3];
    u8 behaviorPhase;
};
struct ArmorTarget {
    u8 pad[0x3c];
    struct ArmorActor *actor;
};
void ActorController_SetBehavior(struct ArmorActor *, s32);
void Armor_SetPlanarVelocity(struct ArmorActor *, s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);

s32 Armor_ReceiveTargetEvent(struct ArmorTarget *target, s32 event, s32 amount)
{
    struct ArmorActor *p = target->actor;
    s32 damage;
    s32 result;
    if (event == 0) {
        damage = amount;
        p->health -= damage;
        if (p->health <= 0) {
            ActorController_SetBehavior(p, 18);
            p->flags |= 0x80;
        } else {
            p->behaviorPhase = 255;
            ActorController_SetBehavior(p, 17);
        }
        SceneHud_SetTimedState(p->actorId, p->health, p->maxHealth, 5, 0, -1);
        if (damage > 0)
            DamageNumberDisplay_Show(p->damageDisplay, damage, 0, &p->damagePosition[0],
                                     &p->damagePosition[2], 1);
    } else if (event == 1) {
        Armor_SetPlanarVelocity(p, (p->direction + 0xa0) & 0xc0, 30);
        if (!(p->flags & 0x80)) {
            p->behaviorPhase = 255;
            ActorController_SetBehavior(p, 15);
        }
    } else {
        goto unrecognized;
    }
    result = 1;
    goto done;
unrecognized:
    result = 0;
done:
    return result;
}
