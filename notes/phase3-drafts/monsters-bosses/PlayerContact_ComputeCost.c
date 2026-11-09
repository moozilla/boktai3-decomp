#include "global.h"
#include "contact_collider.h"
#include "player.h"
void Collision_AccumulateNearbyResponse(void *, void *, s32);
void PlayerCue_ApplyColliderScale(void *, void *);
void Anim_ApplyDurationScales(void *, void *);
/* Select the elemental response and scale damage by the matching player defense. */
#define PlayerContact_ComputeCost sub_0815E034
void PlayerContact_ComputeCost(struct Player *player, struct ContactCollider *input,
                               struct ContactCollider *inputTarget)
{
    struct ContactCollider *source = input;
    struct ContactCollider *target = inputTarget;
    u32 damage;
    u32 flags;
    s8 blocked, vulnerable;
    u32 common;
    if (target->cost)
        return;
    {
        u32 bypassDefense = 0x1000;
        if (input->reactionFlags & bypassDefense)
            damage = source->unknown3C;
        else
            damage = source->unknown3C - PLAYER_BODY_DEFENSE;
    }
    if ((s32)damage <= 0)
        damage = 1;
    flags = input->reactionFlags;
    if (flags & 0x2000)
        blocked = 0;
    else
        blocked = target->reactionFlags & 63;
    vulnerable = target->statRewardFlags;
    common = 63;
    vulnerable &= common;
    if (flags & 64) {
        target->cost = 0;
        target->costFlags = 1;
        Collision_AccumulateNearbyResponse(source, target, target->cost);
        return;
    }
    if (flags & common) {
        u32 offset;
        if (vulnerable & flags)
            target->costFlags = 4;
        else if (blocked & flags)
            target->costFlags = 2;
        else
            target->costFlags = 1;
        switch (flags & 127) {
            case 1:
                offset = (u32) & ((struct Player *)0)->cueDefenses[0];
                break;
            case 2:
                offset = (u32) & ((struct Player *)0)->cueDefenses[1];
                break;
            case 4:
                offset = (u32) & ((struct Player *)0)->cueDefenses[2];
                break;
            case 8:
                offset = (u32) & ((struct Player *)0)->cueDefenses[3];
                break;
            case 16:
                offset = (u32) & ((struct Player *)0)->cueDefenses[4];
                break;
            case 32:
                offset = (u32) & ((struct Player *)0)->cueDefenses[5];
                break;
            default:
                goto apply;
        }
        {
            u8 *attribute = (u8 *)player + offset;
            s32 factor = 5 - *attribute;
            target->cost = damage + ((damage * 5) >> 5) * factor;
        }
    apply:
        Collision_AccumulateNearbyResponse(source, target, damage);
        PlayerCue_ApplyColliderScale(player, target);
        Anim_ApplyDurationScales(source, target);
    } else {
        target->cost = damage;
        target->costFlags = 1;
        Collision_AccumulateNearbyResponse(source, target, damage);
        PlayerCue_ApplyColliderScale(player, target);
        Anim_ApplyDurationScales(source, target);
    }
    if (!target->cost)
        target->cost = 1;
}
