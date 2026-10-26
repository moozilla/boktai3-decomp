#include "global.h"
#include "ram.h"
struct NidhoggrEffectVector {
    u16 x, y, z, pad;
};
struct NidhoggrEffectPair {
    u32 a, b;
};
struct NidhoggrEffectSprite {
    u8 pad[0x10];
    u16 frame;
};
struct NidhoggrEffectCollider {
    u8 pad[6];
    u16 flags;
};
struct NidhoggrDistanceEffect {
    void (*update)(void *), (*destroy)(void *);
    u8 animation[0x1c];
    struct NidhoggrEffectSprite sprite;
    u8 pad38[8];
    struct NidhoggrEffectVector position;
    u8 pad48[0x12c];
    u32 field174;
    s16 speedX, speedY, speedZ;
    u16 pad17E;
    u8 phase, phaseTimer;
    s16 startX;
};
extern const u16 gUnk_086141BC[];
extern const struct NidhoggrEffectPair gUnk_0824F29C, gUnk_0824F2A4;
struct NidhoggrDistanceEffect *SceneEffectPool_AllocateSlot(void *);
void SceneEffect_ResetState(void *);
void Nidhoggr_UpdateDistanceLimitedEffect(void *);
void Nidhoggr_DestroyCollidableEffect(void *);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
void Nidhoggr_CreateDistanceLimitedEffect(void *owner, struct NidhoggrEffectVector *anchor,
                                          struct NidhoggrEffectVector *offset, u8 *boss)
{
    struct NidhoggrDistanceEffect *effect = SceneEffectPool_AllocateSlot(owner);
    u8 *animation;
    struct NidhoggrEffectSprite *sprite;
    struct NidhoggrEffectVector *position;
    u8 *collider;
    if (!effect)
        return;
    SceneEffect_ResetState(effect);
    {
        void (*update)(void *) = Nidhoggr_UpdateDistanceLimitedEffect;
        void (*destroy)(void *) = Nidhoggr_DestroyCollidableEffect;
        effect->update = update;
        effect->destroy = destroy;
    }
    animation = effect->animation;
    AnimData_Load(animation, 0xa0d5);
    AnimData_SelectPaletteBank(animation, 0x5d);
    sprite = &effect->sprite;
    Sprite_Init(sprite, animation, 0);
    {
        u16 zero = 0;
        u8 byteZero = 0;
        sprite->frame = zero;
        effect->phase = byteZero;
        effect->phaseTimer = byteZero;
        position = &effect->position;
        position->x = anchor->x + offset->x;
        position->y = anchor->y + offset->y;
        position->z = anchor->z + offset->z;
        effect->field174 = zero;
        effect->speedX = 0x26;
        effect->speedY = zero;
    }
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    {
        u32 randomValue = gUnk_086141BC[gEffectRandomTableIndex];
        u32 mask = 7;
        effect->speedZ = (randomValue & mask) - 3;
    }
    effect->startX = position->x;
    {
        u32 attack = *(u16 *)(boss + 0x726e) >> 3;
        collider = (u8 *)effect + 0xcc;
        *(struct NidhoggrEffectPair *)(collider + 0x1c) = gUnk_0824F29C;
        *(struct NidhoggrEffectPair *)(collider + 0x14) = gUnk_0824F2A4;
        {
            u32 mask = 4;
            ((struct NidhoggrEffectCollider *)collider)->flags &= ~mask;
        }
        Collider_SetAttackProperties(collider, attack, 0, 0x1000, 0x8000, 4);
    }
    Collider_SetCallback(collider, 0, 0);
    {
        struct NidhoggrEffectCollider *other = (void *)((u8 *)effect + 0x120);
        other->flags |= 4;
        Collider_Refresh(other);
    }
}
