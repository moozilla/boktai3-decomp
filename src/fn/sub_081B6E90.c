#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// foundry draft (worker best 85.7%); what differs is mostly register allocation and instruction order
#define gEffectRandomTable gUnk_0203B400
#define gEffectRandomIndex gUnk_03005308
struct EffectPosition {
    s16 x, y, z, pad;
};
struct EffectTrailSprite {
    struct EffectPosition position;
    u8 pad[0x24];
};
struct EffectCollider {
    u8 pad[6];
    u16 flags;
};
struct ActorEffect {
    u8 entered, state;
    u8 pad2[4];
    u16 timer;
    u8 trailIndex, direction;
    u8 pada[0x5c - 0xa];
    u8 angles[6], radii[6];
    u8 pad68[0xbc - 0x68];
    struct EffectCollider healthCollider;
    u8 padc4[0x12c - 0xc4];
    struct EffectTrailSprite trails[6];
};
struct EffectOwner;
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
s32 Math_Atan2(s32, s32);
void ActorEffect_UpdateRadialTrail(struct ActorEffect *);
u32 ActorEffect_RemoveSprites(struct EffectOwner *, struct ActorEffect *, u32);
static inline s32 NextRandom(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
inline static s32 NextRandom_UpdateRadialExpiry(void)
{
    u32 offset;
    do {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    } while (0);
    offset = gUnk_03005308 * 2;
    return *((const u16 *)(offset + ((u32)gUnk_0203B400)));
}

void ActorEffect_UpdateRadialExpiry(struct EffectOwner *owner, struct ActorEffect *effect,
                                    u32 index)
{
    if (effect->entered) {
        s32 i;
        effect->entered = 0;
        {
            struct EffectCollider *c = &effect->healthCollider;
            c->flags |= 4;
        }
        {
            struct EffectPosition *tail = &effect->trails[1].position;
            struct EffectPosition *head = &effect->trails[0].position;
            effect->direction = Math_Atan2(head->x - tail->x, head->z - tail->z);
        }
        for (i = 0; i <= 5; i++) {
            effect->angles[i] = ((NextRandom_UpdateRadialExpiry() & 31) + 240) + effect->direction;
            effect->radii[i] = (NextRandom_UpdateRadialExpiry() & 15) + 24;
        }
    }
    ActorEffect_UpdateRadialTrail(effect);
    if (effect->timer > 29)
        ActorEffect_RemoveSprites(owner, effect, index);
    else
        effect->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", ActorEffect_UpdateRadialExpiry);
#endif
