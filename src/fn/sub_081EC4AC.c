#include "global.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning
#define gEffectRandomIndex gEffectRandomTableIndex
#define gEffectRandomTable gUnk_086141BC
struct BurstActor {
    u8 pad0[0x268];
    s32 height;
    u8 pad26C[0x278 - 0x26C];
    s16 anchor;
    u8 pad27A[0x388 - 0x27A];
    u32 resourceA;
    u32 resourceB;
};
extern const u16 gUnk_086141BC[];
struct EffectVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 unused : 16;
};
void sub_08113398(struct EffectVector *, struct EffectVector *, u32, u32, u32, u32, u32, u32, u32,
                  u32, u32, u32, u32, u32);
void ActorEffect_SpawnBurstAtAnchor(struct BurstActor *actor, u32 variant)
{
    struct EffectVector position;
    struct EffectVector velocity;
    position.x = actor->anchor;
    position.y = 158;
    position.z = 50;
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 sample = gUnk_086141BC[gEffectRandomTableIndex];
        register u16 one asm("r5") = 1;
        velocity.x = (sample & one) + 2;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        velocity.y = -(gUnk_086141BC[gEffectRandomTableIndex] & one);
        velocity.z = 0;
    }
    if (variant == 0)
        sub_08113398(&position, &velocity, actor->resourceA, actor->resourceB, 5, 0, 0, 0, 2, 42, 0,
                     512, 1, 0);
    else
        sub_08113398(&position, &velocity, actor->resourceA, actor->resourceB, 6, 0, 0, 0, 2, 30, 0,
                     512, 1, 0);
}
