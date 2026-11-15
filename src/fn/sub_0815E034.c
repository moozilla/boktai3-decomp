#include "global.h"
#include "contact_collider.h"
#include "player.h"
void Collision_AccumulateNearbyResponse(void *, void *, s32);
void PlayerCue_ApplyColliderScale(void *, void *);
void Anim_ApplyDurationScales(void *, void *);
static inline u32 AttackFlags(struct ContactCollider *a, u32 mask)
{
    return a->reactionFlags & mask;
}

/* Select the elemental response and scale damage by the matching player defense. */
#define PlayerContact_ComputeCost sub_0815E034
void PlayerContact_ComputeCost(struct Player *player, struct ContactCollider *source,
                               struct ContactCollider *target)
{
    u32 damage;
    u32 flags;
    u32 blocked, vulnerable;

    if (target->cost)
        return;
    if (AttackFlags(source, 0x1000))
        damage = source->unknown3C;
    else
        damage = source->unknown3C - PLAYER_BODY_DEFENSE;
    if ((s32)damage <= 0)
        damage = 1;
    flags = source->reactionFlags;
    if (flags & 0x2000)
        blocked = 0;
    else
        blocked = target->reactionFlags & 63;
    vulnerable = target->statRewardFlags & 63;
    if (flags & 64) {
        target->cost = 0;
        target->costFlags = 1;
        Collision_AccumulateNearbyResponse(source, target, target->cost);
        return;
    }
    if (flags & 63) {
        if (vulnerable & flags)
            target->costFlags = 4;
        else if (blocked & flags)
            target->costFlags = 2;
        else
            target->costFlags = 1;
        switch (flags & 127) {
            case 1:
                target->cost = damage + (5 - player->cueDefenses[0]) * ((damage * 5) >> 5);
                break;
            case 2:
                target->cost = damage + (5 - player->cueDefenses[1]) * ((damage * 5) >> 5);
                break;
            case 4:
                target->cost = damage + (5 - player->cueDefenses[2]) * ((damage * 5) >> 5);
                break;
            case 8:
                target->cost = damage + (5 - player->cueDefenses[3]) * ((damage * 5) >> 5);
                break;
            case 16:
                target->cost = damage + (5 - player->cueDefenses[4]) * ((damage * 5) >> 5);
                break;
            case 32:
                target->cost = damage + (5 - player->cueDefenses[5]) * ((damage * 5) >> 5);
                break;
        }
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
