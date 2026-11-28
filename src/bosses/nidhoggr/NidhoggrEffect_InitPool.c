#include "global.h"

#define gScenePanelEffectColliderShape gUnk_0824F184

struct NidhoggrEffectSlot {
    u32 state;
    u32 flags;
    u8 unknown8[0x6C - 8];
};
struct NidhoggrCollisionEffect {
    u32 state;
    u32 flags;
    u8 unknown8[0xCC - 8];
    u8 colliderCC[0x54];
    u8 collider120[0x54];
    u8 unknown174[0x188 - 0x174];
};
struct NidhoggrEffectPool {
    struct NidhoggrEffectSlot effects[24];
    void *resource;
    struct NidhoggrCollisionEffect collisionEffects[48];
    u32 position[2];
    void *script;
};
extern u8 gScenePanelEffectColliderShape[];
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void *ScriptProc_Start(u16, u16);

void NidhoggrEffect_InitPool(struct NidhoggrEffectPool *pool)
{
    s32 i;
    struct NidhoggrEffectSlot *effect;
    struct NidhoggrCollisionEffect *collision;
    pool->script = ScriptProc_Start(0x922E, 0xBB30);
    pool->resource = 0;
    for (i = 0; (u32)i < 2; i++)
        pool->position[i] = 0;
    effect = pool->effects;
    for (i = 24; i > 0; i--, effect++) {
        effect->state = 0;
        effect->flags = 0;
    }
    collision = pool->collisionEffects;
    for (i = 48; i > 0; i--, collision++) {
        collision->state = 0;
        collision->flags = 0;
        Collider_Init(collision->colliderCC, 0, 0x2001, 0, 0x10, gScenePanelEffectColliderShape,
                      gScenePanelEffectColliderShape);
        Collider_Init(collision->collider120, 0, 0x4001, 0, 0x10, gScenePanelEffectColliderShape,
                      gScenePanelEffectColliderShape);
    }
}
