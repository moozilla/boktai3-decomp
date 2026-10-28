#include "global.h"
#include "ram.h"
struct NidhoggrEffectVector {
    u16 x, y, z, pad;
};
struct NidhoggrEffectPair {
    u32 a, b;
};
struct NidhoggrEffectCollider {
    u8 pad[6];
    u16 flags;
};
struct NidhoggrAttachedEffect {
    void (*update)(void *);
    void (*destroy)(void *);
    u8 animation[0x1c];
    u8 sprite[0x1c];
    struct NidhoggrEffectVector position;
    u8 pad48[0x8c];
    u16 flagsD4, colliderFlags;
    u8 padD8[8];
    struct NidhoggrEffectPair boundsE0, boundsE8;
    u8 padF0[0x36];
    u16 flags126;
    u8 pad128[0x4c];
    struct NidhoggrEffectVector *anchor;
    struct NidhoggrEffectVector offset;
};
extern u32 gUnk_02000560;
extern const struct NidhoggrEffectPair gUnk_0824F264, gUnk_0824F26C;
struct NidhoggrAttachedEffect *SceneEffectPool_AllocateSlot(void *);
void SceneEffect_ResetState(void *);
void Nidhoggr_UpdateDetachedAttackEffect(void *);
void Nidhoggr_DestroyCollidableEffect(void *);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
void Sound_PlaySongChecked(u32);
void Nidhoggr_CreateAttachedAttackEffect(u8 *owner, struct NidhoggrEffectVector *anchor,
                                         void *offsetArg, u8 *boss)
{
    struct NidhoggrEffectVector *offset = offsetArg;
    struct NidhoggrAttachedEffect *effect;
    u8 *animation;
    u8 *collider;
    u32 mask = 0x400;
    if ((gEngineFlags | gScenePauseFlags) & mask)
        return;
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return;
    }
    effect = SceneEffectPool_AllocateSlot(owner);
    if (!effect)
        return;
    SceneEffect_ResetState(effect);
    {
        void (*update)(void *) = Nidhoggr_UpdateDetachedAttackEffect;
        void (*destroy)(void *) = Nidhoggr_DestroyCollidableEffect;
        effect->update = update;
        effect->destroy = destroy;
    }
    animation = effect->animation;
    AnimData_Load(animation, 0xc7ed);
    AnimData_SelectPaletteBank(animation, 0x1a9);
    Sprite_Init(effect->sprite, animation, 0);
    Anim_RestartSequence((u8 *)effect + 0x50, *(void **)(owner + 0x53ac), 2, 0, 8);
    effect->anchor = anchor;
    effect->offset = *offset;
    {
        struct NidhoggrEffectVector *position = &effect->position;
        position->x = anchor->x + offset->x;
        position->y = anchor->y + offset->y;
        position->z = anchor->z + offset->z;
    }
    {
        u16 attack = *(u16 *)(boss + 0x726e);
        collider = animation + 0xc4;
        *(struct NidhoggrEffectPair *)(collider + 0x1c) = gUnk_0824F264;
        *(struct NidhoggrEffectPair *)(collider + 0x14) = gUnk_0824F26C;
        *(u16 *)(collider + 6) |= 4;
        Collider_SetAttackProperties(collider, attack, 5, 0x10, 0x40020, 0x1e);
    }
    Collider_SetCallback(collider, 0, 0);
    {
        struct NidhoggrEffectCollider *other = (void *)((u8 *)effect + 0x120);
        other->flags |= 4;
        Collider_Refresh(other);
    }
    Sound_PlaySongChecked(0x1b6);
}
