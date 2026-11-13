#include "scene_panel_effect_pool.h"

void sub_0818ECFC(struct ScenePanelEffectPool *);

void Nidhoggr_ClearEffectPools(struct ScenePanelEffectPool *);

void ScenePanelEffect_InitPool(struct ScenePanelEffectPool *);

void PlayerStats_AddContactHundredths(u32, u32, s32);

void ExplosionEffect_Create(u32, u32, u32, u32, u32);

struct ScenePanelOwnedEffect *SceneEffectPool_AllocateSlot(void *);

void SceneEffect_ResetState(void *);

void Nidhoggr_DestroyShadowedEffect(void *);

void Nidhoggr_OnEffectCollision(void *);

void Anim_RestartSequence(void *, void *, u32, u32, u32);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void Collider_SetBodyProperties(void *, s32, s32, s32);

void ScaledShadow_InitializeDefaultSmall(void *, void *);

void ScaledShadow_Disable(void *);

extern u32 gUnk_02000560;

extern const u16 gUnk_086141BC[];

extern const struct ContactPosition gUnk_0824F2D4, gUnk_0824F2CC;

extern struct ContactPosition gUnk_0824F2DC;

static inline void StepSequence(struct ScenePanelOwnedEffect *slot)
{
    PlayerCue_StepSequence(&slot->sprite, &slot->sequence);
}

static inline s32 RandomValue(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

static inline void AddPosition(struct ContactPosition *out, struct ContactPosition *position)
{
    out->x += position->x;
    out->y += position->y;
    out->z += position->z;
}

static inline u32 ReadBlockFlag(u32 mask)
{
    return gUnk_02000560 & mask;
}

u32 ScenePanelEffect_UpdateTask(struct ScenePanelEffect *effect)
{
    sub_0818ECFC(&effect->pool);
    return 0;
}

u32 ScenePanelEffect_DestroyTask(struct ScenePanelEffect *effect)
{
    Nidhoggr_ClearEffectPools(&effect->pool);
    return 0;
}

struct ScenePanelEffect *ScenePanelEffect_Create(u32 impactVariant, u32 contactHundredths)
{
    struct ScenePanelEffect *effect = Task_Create(8, sizeof(*effect));
    if (effect != 0) {
        Task_SetCallbacks(effect, (void (*)(void *))ScenePanelEffect_UpdateTask,
                          (void (*)(void *))ScenePanelEffect_DestroyTask);
        ScenePanelEffect_InitPool(&effect->pool);
        effect->impactVariant = impactVariant;
        effect->contactHundredths = contactHundredths;
    }
    return effect;
}

void ScenePanelEffect_OnContact(u32 first, u32 second, struct ScenePanelOwnedEffect *slot)
{
    struct ScenePanelEffect *owner = slot->owner;
    slot->state = 0xff;
    PlayerStats_AddContactHundredths(first, second, owner->contactHundredths);
}

void ScenePanelEffect_CreatePositionEffect(const struct ContactPosition *position,
                                           struct ScenePanelEffect *effect)
{
    ExplosionEffect_Create((u32)position, 0x10, effect->impactVariant, 0xa, 0);
}

void Nidhoggr_UpdateOwnedSeekingEffect(struct ScenePanelOwnedEffect *owner)
{
    struct ScenePanelEffectDelta delta;
    s32 dx, dy, dz, angle;
    u8 *stateAddress;
    StepSequence(owner);
    stateAddress = &owner->state;
    if (*stateAddress == 0)
        goto waiting;
    if (*stateAddress == 255)
        goto impact;
    {
        {
            struct GameState **targetSlot = &gGameState;
            struct GameState *target = *targetSlot;
            s32 targetX = target->positionX;
            dx = targetX - (s16)owner->sprite.position.x;
            dy = target->positionY - (s16)owner->sprite.position.y;
            dz = target->positionZ - (s16)owner->sprite.position.z;
        }
        angle = Math_Atan2(dx, dz);
        if (RandomValue() > 0xdfff)
            goto fastMotion;
        delta.x = gUnk_086149C4[(angle + 64) & 255] >> 10;
        delta.y = 0;
        delta.z = gUnk_086149C4[angle & 255] >> 10;
        goto spread;
    waiting: {
        if ((s16)++owner->motion.x > 8)
            owner->motion.x = 8;
        if (owner->age > 30) {
            (*stateAddress)++;
            owner->age = 0;
        }
    }
        return;
    impact: {
        struct ScenePanelEffectDelta impact;
        void *boss = owner->owner;
        impact.x = 0;
        impact.y = -96;
        impact.z = 0;
        AddPosition((struct ContactPosition *)&impact, &owner->sprite.position);
        ScenePanelEffect_CreatePositionEffect((struct ContactPosition *)&impact, boss);
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
        return;
    fastMotion:
        delta.x = gUnk_086149C4[(angle + 64) & 255] >> 9;
        delta.y = 0;
        delta.z = gUnk_086149C4[angle & 255] >> 9;
    spread: {
        angle = 255 & RandomValue();
        delta.x += gUnk_086149C4[(angle + 64) & 255] >> 10;
        delta.z += gUnk_086149C4[angle & 255] >> 10;
    }
        owner->motion = *(struct ContactPosition *)&delta;
        if (owner->age == 0)
            owner->state++;
        if (owner->state > 1 && owner->age > 88) {
            struct ScenePanelEffectDelta impact;
            void *boss = owner->owner;
            impact.x = 0;
            impact.y = -128;
            impact.z = 0;
            AddPosition((struct ContactPosition *)&impact, &owner->sprite.position);
            ScenePanelEffect_CreatePositionEffect((struct ContactPosition *)&impact, boss);
            owner->update = 0;
            if (owner->onComplete) {
                Callback_InvokeActor(owner, owner->onComplete);
                owner->onComplete = 0;
            }
        } else {
            dx += 48;
            dy += 256;
            dz += 48;
            if ((u32)dx <= 96 && dy >= 0 && dy <= 256 && dz >= 0 && dz <= 96)
                owner->state = 255;
        }
    }
}

void Nidhoggr_CreateOwnedSeekingEffect(struct ScenePanelEffect *owner,
                                       struct ContactPosition *anchor,
                                       struct ContactPosition *offset)
{
    struct ScenePanelOwnedEffect *effect;
    u8 *animation, *collider, *shadow;
    struct ContactPosition *position;
    void (*callback)(void *);
    u32 mask = 0x400;
    u32 zero;
    if ((gEngineFlags | gScenePauseFlags) & mask)
        return;
    zero = ReadBlockFlag(4);
    if (zero)
        return;
    effect = SceneEffectPool_AllocateSlot(&owner->pool);
    if (!effect)
        return;
    SceneEffect_ResetState(effect);
    {
        void (*update)(void *) = Nidhoggr_UpdateOwnedSeekingEffect;
        void (*destroy)(void *) = Nidhoggr_DestroyShadowedEffect;
        effect->update = update;
        effect->onComplete = destroy;
    }
    animation = effect->animationData;
    AnimData_Load(animation, 0xc7ed);
    AnimData_SelectPaletteBank(animation, 0x1a6);
    Sprite_Init(&effect->sprite, animation, 0);
    Anim_RestartSequence(&effect->sequence, owner->pool.sequenceResource, 0, 0, zero);
    position = &effect->sprite.position;
    position->x = anchor->x + offset->x;
    position->y = anchor->y + offset->y;
    position->z = anchor->z + offset->z;
    effect->owner = owner;
    animation = (u8 *)&effect->attackCollider;
    collider = animation;
    ((struct ContactCollider *)collider)->bounds = gUnk_0824F2D4;
    ((struct ContactCollider *)collider)->offset = gUnk_0824F2DC;
    {
        struct ContactCollider *attack = (void *)collider;
        u32 collisionMask;
        collisionMask = 4;
        attack->flags |= collisionMask;
        Collider_SetAttackProperties(collider, 0, 0, 0, zero, zero);
        callback = Nidhoggr_OnEffectCollision;
        Collider_SetCallback(collider, callback, effect);
        {
            u32 bodyOffset = 0x120;
            struct ContactCollider *body;
            collider = (u8 *)&effect->bodyCollider;
            ((struct ContactCollider *)collider)->bounds = gUnk_0824F2CC;
            ((struct ContactCollider *)collider)->offset = gUnk_0824F2DC;
            body = (void *)collider;
            body->flags &= ~collisionMask;
            {
                u32 add = 0x1008;
                body->flags |= add;
            }
            Collider_SetBodyProperties(collider, 1, bodyOffset, 16);
        }
    }
    Collider_SetCallback(collider, ScenePanelEffect_OnContact, effect);
    Collider_Refresh(collider);
    shadow = effect->shadow;
    ScaledShadow_InitializeDefaultSmall(shadow, position);
    Sound_PlaySongChecked(0x1b9);
}
