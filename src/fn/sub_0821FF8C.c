#include "collision_response.h"
#include "game_state.h"
s32 Div(s32, s32);
static inline s32 ScaleSigned(s32 value)
{
    if (value >= 0)
        value >>= 6;
    else
        value = -(-value >> 6);
    return value;
}
static inline void ApplyScales(struct ContactCollider *a, struct ContactCollider *b)
{
    if (a->damageScale != 64) {
        if (a->statRewardFlags == 16) {
            s32 factor = gGameState->unk876;
            b->cost += Div((b->cost << 1) * factor, 100);
        } else
            b->cost = ScaleSigned(b->cost * a->damageScale);
    }
    if (b->damageScale != 64)
        b->cost = ScaleSigned(b->cost * b->damageScale);
    if (!b->cost)
        b->cost = 1;
}
static inline void Nearby(struct ContactCollider *a, struct ContactCollider *b, u32 amount)
{
    if ((b->flags & 0x100) && (a->flags & 0x100)) {
        s32 delta = b->direction - a->direction;
        if (delta < 0)
            delta = -delta;
        if (delta <= 32) {
            b->cost += amount >> 2;
            b->costFlags |= 8;
        }
    }
}
static inline u32 AttackFlags(struct ContactCollider *a, u32 mask)
{
    return a->reactionFlags & mask;
}
void Collision_CalculateDamageResponse(struct ContactCollider *a, struct ContactCollider *b)
{
    u32 damage;
    u32 defense, kind, attack;
    if (b->cost)
        return;
    if (AttackFlags(a, 0x1000))
        damage = a->unknown3C;
    else
        damage = a->unknown3C;
    if ((s32)damage <= 0)
        damage = 1;
    attack = a->reactionFlags;
    if (attack & 0x2000)
        defense = 0;
    else
        defense = b->reactionFlags & 63;
    kind = b->statRewardFlags & 63;
    if (attack & 64) {
        b->cost = 0;
        b->costFlags = 1;
        Nearby(a, b, b->cost);
    } else if (attack & 63) {
        if (kind & attack) {
            u32 v = (damage * 5) >> 5;
            b->cost = damage + v * 5;
            b->costFlags = 4;
        } else if (defense & attack) {
            u32 v = (damage * 5) >> 5;
            b->cost = damage - v * 5;
            b->costFlags = 2;
        } else {
            b->cost = damage + ((damage * 5) >> 5);
            b->costFlags = 1;
        }
        Nearby(a, b, damage);
        ApplyScales(a, b);
    } else {
        b->cost = damage;
        b->costFlags = 1;
        Nearby(a, b, damage);
        ApplyScales(a, b);
    }
}
