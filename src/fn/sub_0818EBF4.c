#include "global.h"

#define ScenePanelEffect_InitPool      sub_0818EBF4
#define gScenePanelEffectColliderShape gUnk_0824F184

struct ScenePanelEffectSlot {
    u32 state;
    u32 flags;
    u8 unknown8[0x6C - 8];
};
struct ScenePanelCollisionEffect {
    u32 state;
    u32 flags;
    u8 unknown8[0xCC - 8];
    u8 colliderCC[0x54];
    u8 collider120[0x54];
    u8 unknown174[0x188 - 0x174];
};
struct ScenePanelEffectPool {
    struct ScenePanelEffectSlot effects[24];
    void *resource;
    struct ScenePanelCollisionEffect collisionEffects[48];
    u32 position[2];
    void *script;
};
extern u8 gScenePanelEffectColliderShape[];
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void *ScriptProc_Start(u16, u16);

void sub_0818EBF4(struct ScenePanelEffectPool *pool)
{
    s32 i;
    u32 *word;
    struct ScenePanelEffectSlot *effect;
    struct ScenePanelCollisionEffect *collision;
    pool->script = ScriptProc_Start(0x922E, 0xBB30);
    pool->resource = 0;
    i = 0;
    {
        u32 zero = 0;
        word = pool->position;
        do {
            *word++ = zero;
            i++;
        } while ((u32)i <= 1);
    }
    effect = pool->effects;
    i = 24;
    do {
        effect->state = 0;
        effect->flags = 0;
        i--;
        effect++;
    } while (i > 0);
    collision = pool->collisionEffects;
    i = 48;
    do {
        collision->state = 0;
        collision->flags = 0;
        Collider_Init(collision->colliderCC, 0, 0x2001, 0, 0x10, gScenePanelEffectColliderShape,
                      gScenePanelEffectColliderShape);
        Collider_Init(collision->collider120, 0, 0x4001, 0, 0x10, gScenePanelEffectColliderShape,
                      gScenePanelEffectColliderShape);
        i--;
        collision++;
    } while (i > 0);
}
