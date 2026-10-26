#include "global.h"
#include "game_state.h"
struct CollisionEntry {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x2c];
    u32 kind, attack;
    u16 damage, response;
    u16 pad40;
    u8 layer, responseFlags;
    u8 pad44[4];
    s8 scale;
};
s32 Div(s32, s32);
static inline s32 ScaleSigned(s32 value)
{
    if (value >= 0)
        value >>= 6;
    else
        value = -(-value >> 6);
    return value;
}
static inline void ApplyScales(struct CollisionEntry *a, struct CollisionEntry *b)
{
    if (a->scale != 64) {
        if (a->kind == 16) {
            s32 factor = gGameState->unk876;
            b->response += Div((b->response << 1) * factor, 100);
        } else
            b->response = ScaleSigned(b->response * a->scale);
    }
    if (b->scale != 64)
        b->response = ScaleSigned(b->response * b->scale);
    if (!b->response)
        b->response = 1;
}
static inline void Nearby(struct CollisionEntry *a, struct CollisionEntry *b, u32 amount)
{
    if ((b->flags & 0x100) && (a->flags & 0x100)) {
        s32 delta = b->layer - a->layer;
        if (delta < 0)
            delta = -delta;
        if (delta <= 32) {
            b->response += amount >> 2;
            b->responseFlags |= 8;
        }
    }
}
static inline u32 AttackFlags(struct CollisionEntry *a, u32 mask)
{
    return a->attack & mask;
}
void Collision_CalculateDamageResponse(struct CollisionEntry *a, struct CollisionEntry *b)
{
    u32 damage;
    u32 defense, kind, attack;
    if (b->response)
        return;
    if (AttackFlags(a, 0x1000))
        damage = a->damage;
    else
        damage = a->damage;
    if ((s32)damage <= 0)
        damage = 1;
    attack = a->attack;
    if (attack & 0x2000)
        defense = 0;
    else
        defense = b->attack & 63;
    kind = b->kind & 63;
    if (attack & 64) {
        b->response = 0;
        b->responseFlags = 1;
        Nearby(a, b, b->response);
    } else if (attack & 63) {
        if (kind & attack) {
            u32 v = (damage * 5) >> 5;
            b->response = damage + v * 5;
            b->responseFlags = 4;
        } else if (defense & attack) {
            u32 v = (damage * 5) >> 5;
            b->response = damage - v * 5;
            b->responseFlags = 2;
        } else {
            b->response = damage + ((damage * 5) >> 5);
            b->responseFlags = 1;
        }
        Nearby(a, b, damage);
        ApplyScales(a, b);
    } else {
        b->response = damage;
        b->responseFlags = 1;
        Nearby(a, b, damage);
        ApplyScales(a, b);
    }
}
