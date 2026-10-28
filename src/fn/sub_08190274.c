// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct NidhoggrEffectVector {
    u16 x, y, z, pad;
};
struct NidhoggrEffectPair {
    u32 a, b;
};
struct NidhoggrFallingEffect {
    void (*update)(void *), (*destroy)(void *);
    u8 animation[0x1c];
    u8 sprite[0x1c];
    struct NidhoggrEffectVector position;
    u8 unknown48[0x174 - 0x48];
    struct NidhoggrEffectVector *anchor;
    struct NidhoggrEffectVector offset;
    u8 unknown180[4];
    u8 *boss;
};
extern u32 gUnk_02000560;
extern const struct NidhoggrEffectPair gUnk_0824F27C, gUnk_0824F274;
extern struct NidhoggrEffectPair gUnk_0824F284;
struct NidhoggrFallingEffect *SceneEffectPool_AllocateSlot(void *);
void SceneEffect_ResetState(void *);
void sub_0818FEE4(void *);
void Nidhoggr_DestroyShadowedEffect(void *);
void Nidhoggr_OnEffectCollision(void *);
void SceneAnimation_OnEffectContact(void *);
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_Refresh(void *);
void Sound_PlaySongChecked(u32);
void ScaledShadow_InitializeDefaultSmall(void *, void *);
void ScaledShadow_Disable(void *);
static inline u32 ReadBlockFlag(u32 mask)
{
    return gUnk_02000560 & mask;
}
// confidence=low: allocates a player-seeking effect with two collision channels and a small shadow.
#define Nidhoggr_CreateSeekingAttackEffect sub_08190274
void Nidhoggr_CreateSeekingAttackEffect(u8 *owner, struct NidhoggrEffectVector *anchor,
                                        struct NidhoggrEffectVector *offset, u8 *boss)
{
    struct NidhoggrFallingEffect *effect;
    u8 *animation, *collider, *shadow;
    struct NidhoggrEffectVector *position;
    void (*callback)(void *);
    u32 mask = 0x400;
    u32 zero;
    if ((gEngineFlags | gScenePauseFlags) & mask)
        return;
    zero = ReadBlockFlag(4);
    if (zero)
        return;
    effect = SceneEffectPool_AllocateSlot(owner);
    if (!effect)
        return;
    SceneEffect_ResetState(effect);
    {
        void (*update)(void *) = sub_0818FEE4;
        void (*destroy)(void *) = Nidhoggr_DestroyShadowedEffect;
        effect->update = update;
        effect->destroy = destroy;
    }
    animation = effect->animation;
    AnimData_Load(animation, 0xc7ed);
    AnimData_SelectPaletteBank(animation, 0x1a6);
    Sprite_Init(effect->sprite, animation, 0);
    Anim_RestartSequence((u8 *)effect + 0x50, *(void **)(owner + 0x53ac), 0, 0, zero);
    position = &effect->position;
    position->x = anchor->x + offset->x;
    position->y = anchor->y + offset->y;
    position->z = anchor->z + offset->z;
    effect->boss = boss;
    animation += 0xc4;
    collider = animation;
    *(struct NidhoggrEffectPair *)(collider + 0x1c) = gUnk_0824F27C;
    *(struct NidhoggrEffectPair *)(collider + 0x14) = gUnk_0824F284;
    {
        u32 flags = *(u16 *)(collider + 6);
        register u32 collisionMask asm("r5");
        collisionMask = 4;
        *(u16 *)(collider + 6) = flags | collisionMask;
        Collider_SetAttackProperties(collider, 0, 0, 0, zero, zero);
        callback = Nidhoggr_OnEffectCollision;
        Collider_SetCallback(collider, callback, effect);
        {
            u32 bodyOffset = 0x120;
            collider = (u8 *)effect + bodyOffset;
            *(struct NidhoggrEffectPair *)(collider + 0x1c) = gUnk_0824F274;
            *(struct NidhoggrEffectPair *)(collider + 0x14) = gUnk_0824F284;
            collisionMask = ~collisionMask;
            asm("" : "+r"(collisionMask));
            collisionMask &= *(u16 *)(collider + 6);
            collisionMask |= 0x1008;
            *(u16 *)(collider + 6) = collisionMask;
            Collider_SetBodyProperties(collider, 1, bodyOffset, 16);
        }
    }
    Collider_SetCallback(collider, SceneAnimation_OnEffectContact, effect);
    Collider_Refresh(collider);
    shadow = (u8 *)effect + 0x60;
    ScaledShadow_InitializeDefaultSmall(shadow, position);
    Sound_PlaySongChecked(0x1b9);
}
