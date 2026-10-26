#define SpriteEffect_GetOrCreatePool  SceneActor14_EnsureSingleton
#define SpriteEffect_GetFacingVariant SceneActor14_GetFacingAndFlip
#include "global.h"
#include "ram.h"

struct EffectMotion {
    u32 xy, z;
};
struct EffectSourceObject {
    u8 unk00[9];
    u8 facing;
    u8 unk0A[2];
    struct EffectMotion position;
};
struct SpriteEffect {
    u32 flags;
    u8 unk04[3];
    u8 layer;
    u8 unk08[4];
    void *animation;
    u16 frame;
    u8 unk12[0xa];
    struct EffectMotion displayPosition;
    u8 unk24[8];
    struct EffectMotion position;
    struct EffectMotion motion;
    u8 collider[0x54];
    u8 shadow[0x40];
    s32 timer;
    u16 state;
    u8 active;
    s8 slot;
};
struct SpriteEffectPool {
    u8 unk00[0x18];
    u8 spriteData[0x1c];
    u32 activeEffects;
    struct SpriteEffect effects[8];
};
struct SpriteEffectPool *SceneActor14_EnsureSingleton(void);
struct SpriteEffect *SpriteEffectPool_AllocateSlot(struct SpriteEffectPool *);
void SceneActor14_GetFacingAndFlip(struct EffectSourceObject *, u8 *, u8 *);
void Shadow_Initialize(void *, void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void SpriteEffect_InitCollider(struct SpriteEffect *, u32, u32, u32, u32);

s32 SpriteEffect_SpawnWithMotion(struct EffectSourceObject *source,
                                 struct EffectSourceObject *target, struct EffectMotion *motion,
                                 u32 a, u32 b, u32 c, u32 d)
{
    struct SpriteEffectPool *pool;
    struct SpriteEffect *effect;
    u8 variant, flip;
    pool = gSpriteEffectPool;
    if (!pool) {
        pool = SceneActor14_EnsureSingleton();
        if (!pool)
            return -1;
    }
    effect = SpriteEffectPool_AllocateSlot(pool);
    if (!effect)
        return -1;
    effect->active = 1;
    SceneActor14_GetFacingAndFlip(source, &variant, &flip);
    effect->position = source->position;
    effect->state = 0;
    effect->motion = *motion;
    Shadow_Initialize(effect->shadow, &effect->position, 0);
    effect->displayPosition = effect->position;
    Sprite_Init(effect, pool->spriteData, 0);
    effect->frame = 3;
    AnimData_SelectPaletteBank(effect->animation, 0x2c4);
    effect->flags |= 1;
    effect->layer = 2;
    SpriteEffect_InitCollider(effect, a, b, c, d);
    return effect->slot;
}
