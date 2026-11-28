#include "global.h"
#include "scene/collision/contact_collider.h"
#include "system/game_state/game_state.h"
/* Anim_ApplyDurationScales: Scales a contact's damage by both colliders' damage multipliers */

s32 Div(s32, s32);

#define SCALE64(v) ((v) >= 0 ? (v) >> 6 : -(-(v) >> 6))

void Anim_ApplyDurationScales(struct ContactCollider *source, struct ContactCollider *target)
{
    s32 damage;
    if (source->damageScale != 0x40) {
        if (source->statRewardFlags == 0x10) {
            s32 level = gGameState->unk876;
            target->cost += Div((target->cost << 1) * level, 100);
        } else {
            damage = target->cost * source->damageScale;
            damage = SCALE64(damage);
            target->cost = damage;
        }
    }
    if (target->damageScale != 0x40) {
        damage = target->cost * target->damageScale;
        damage = SCALE64(damage);
        target->cost = damage;
    }
}
