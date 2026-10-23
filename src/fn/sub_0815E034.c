// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct ContactCostCollider {
    u8 pad0[0x34];
    u32 vulnerability, reaction;
    u16 amount, cost;
    u8 pad40[3], mode;
};
void Collision_AccumulateNearbyResponse(void *, void *, s32);
void PlayerCue_ApplyColliderScale(void *, void *);
void Anim_ApplyDurationScales(void *, void *);
static inline u32 ReadFlags(struct ContactCostCollider *source, u32 mask)
{
    u32 flags = source->reaction;
    asm("" : : "r"(mask & flags));
    return flags;
}
// confidence=low: selects contact response mode and scales cost by one of six player attributes.
#define PlayerContact_ComputeCost sub_0815E034
void PlayerContact_ComputeCost(u8 *player, struct ContactCostCollider *input,
                               struct ContactCostCollider *inputTarget)
{
    register struct ContactCostCollider *source asm("r8") = input;
    struct ContactCostCollider *target = inputTarget;
    register u32 damage asm("r2");
    register u32 flags asm("r1");
    u32 blocked, vulnerable;
    register u32 common asm("r6");
    if (target->cost)
        return;
    flags = ReadFlags(source, 0x1000);
    damage = source->amount;
    if ((s32)damage <= 0)
        damage = 1;
    if (flags & 0x2000)
        blocked = 0;
    else
        blocked = target->reaction & 63;
    vulnerable = target->vulnerability;
    common = 63;
    vulnerable &= common;
    if (flags & 64) {
        target->cost = 0;
        target->mode = 1;
        Collision_AccumulateNearbyResponse(source, target, target->cost);
        return;
    }
    if (flags & common) {
        register u32 offset asm("r1");
        if (vulnerable & flags)
            target->mode = 4;
        else if (blocked & flags)
            target->mode = 2;
        else
            target->mode = 1;
        switch (flags & 127) {
            case 1:
                offset = 0x450;
                break;
            case 2:
                offset = 0x451;
                break;
            case 4:
                offset = 0x452;
                break;
            case 8:
                offset = 0x453;
                break;
            case 16:
                offset = 0x454;
                break;
            case 32:
                offset = 0x455;
                break;
            default:
                goto apply;
        }
        {
            register u8 *attribute asm("r0") = player + offset;
            s32 factor = 5 - *attribute;
            target->cost = damage + ((damage * 5) >> 5) * factor;
        }
    apply:
        Collision_AccumulateNearbyResponse(source, target, damage);
        PlayerCue_ApplyColliderScale(player, target);
        Anim_ApplyDurationScales(source, target);
    } else {
        target->cost = damage;
        target->mode = 1;
        Collision_AccumulateNearbyResponse(source, target, damage);
        PlayerCue_ApplyColliderScale(player, target);
        Anim_ApplyDurationScales(source, target);
    }
    if (!target->cost)
        target->cost = 1;
}
