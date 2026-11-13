#include "scene_scheduled_effect.h"
#include "global.h"
#include "ram.h"
#include "engine.h"
#include "contact_actor.h"

u8 SceneActor14_UpdateAnimState(struct SceneScheduledEffect *);

void Collider_SetPosition(void *, void *, u32);

s32 Collider_Refresh(void *);

void SceneScheduledEffect_UpdateEntry(struct SceneScheduledEffect *s)
{
    if (SceneActor14_UpdateAnimState(s) == 0) {
        if ((s16)s->variant == 1) {
            struct ContactCollider *p = &s->collider;
            Collider_SetPosition(p, &s->position, 0);
            Collider_Refresh(p);
        }
        s->timer++;
    }
}

/* SceneScheduledEffect_FinalizeEntry: Empty hook beside the entry updater; entry finalization role is tentative */

void SceneScheduledEffect_FinalizeEntry(void)
{
}

void Anim_Remove(void *);

u32 TerrainOverlay_QueueRemoveActor(u32 *);

void SceneBody_Unregister(void *);

static inline u8 HasSlot(u32 *occupied, u32 bit)
{
    if (*occupied & bit)
        return 1;
    return 0;
}

s32 AnimatedBodyPool_Destroy(struct SceneScheduledEffectPool *pool)
{
    s32 i = 0;
    u32 *occupied = &pool->occupied;
    struct SceneScheduledEffect *slot;
    u16 *flags = &pool->slots[0].flags;
    slot = pool->slots;
    do {
        struct SceneActorBody *bodies = &pool->slots[0].body;
        if (HasSlot(occupied, 1 << i)) {
            Anim_Remove(&slot[i]);
            if (*(u16 *)((u8 *)flags + i * sizeof(*slot)) & 1)
                TerrainOverlay_QueueRemoveActor((void *)((u32)bodies + i * sizeof(*slot)));
            SceneBody_Unregister(&slot[i].body);
        }
        i++;
    } while (i <= 3);
    gAnimatedBodyPool = 0;
    return 0;
}

extern u32 gUnk_02000560;

extern void *gUnk_08610704[];

s32 Callback_InvokeActor(void *, void *);

static inline u8 Occupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}

static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}

static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}

s32 StateSpritePool_Tick(struct SceneScheduledEffectPool *pool)
{
    struct SceneScheduledEffect *start = pool->slots;
    struct SceneScheduledEffect *slot;
    u32 state;
    s32 i;
    u32 mask;
    void **table;
    if (pool->occupied) {
        if (!Paused()) {
            if (!Locked()) {
                i = 0;
                mask = (u32)start + sizeof(pool->slots);
                table = gUnk_08610704;
                state = (u32)&pool->slots[0].state;
                slot = start;
                for (; i < 4;) {
                    if (Occupied((u32 *)mask, i))
                        Callback_InvokeActor(slot, table[*(s16 *)state]);
                    {
                        u32 stride = sizeof(*slot);
                        state += stride;
                        slot = (void *)((u32)slot + stride);
                    }
                    i++;
                }
            }
        }
    }
    return 0;
}

// confidence=low: allocates one of four 0x144-byte slots, recording its index at 0x13E.

struct SceneScheduledEffect *SceneScheduledEffect_Allocate(struct SceneScheduledEffectPool *pool)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (!Occupied(&pool->occupied, i)) {
            struct SceneScheduledEffect *slot = &pool->slots[i];
            pool->occupied |= 1 << i;
            slot->slot = i;
            return slot;
        }
    }
    return 0;
}

void SceneScheduledEffect_OnColliderContact(void)
{
}

struct ColliderVector {
    s16 x, y, z;
};

extern const struct ColliderVector gUnk_08610710, gUnk_08610718, gUnk_08610720;

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);

void Collider_SetCallback(void *, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void SceneScheduledEffect_OnColliderContact(void);

void StateSprite_InitCollider(struct SceneScheduledEffect *sprite, s32 side, s32 type, s32 a, s32 b,
                              s32 c)
{
    struct ColliderVector size, offset;
    struct ContactCollider *collider;
    if (side == 0) {
        switch (type) {
            case 0:
                size = gUnk_08610710;
                break;
            case 1:
                size = gUnk_08610718;
                break;
            case 2:
                size = gUnk_08610720;
                break;
        }
        offset.x = 0;
        offset.y = 0;
        offset.z = size.z;
        side = 0x40;
    } else {
        switch (type) {
            case 0:
                size.x = gUnk_08610710.z;
                size.y = gUnk_08610710.y;
                size.z = gUnk_08610710.x;
                break;
            case 1:
                size.x = gUnk_08610718.z;
                size.y = gUnk_08610718.y;
                size.z = gUnk_08610718.x;
                break;
            case 2:
                size.x = gUnk_08610720.z;
                size.y = gUnk_08610720.y;
                size.z = gUnk_08610720.x;
                break;
        }
        offset.x = size.x;
        offset.y = 0;
        offset.z = 0;
        side = 0;
    }
    collider = &sprite->collider;
    Collider_Init(collider, 0, 0x2101, 0, 0x20, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0x400200, 0x200004, c);
    Collider_SetPosition(collider, &sprite->position, 0);
    Collider_SetCallback(collider, SceneScheduledEffect_OnColliderContact, sprite);
    sprite->collider.direction = side;
}

extern const u8 gUnk_086106F0[];

void StateSprite_InitAnimation(struct SceneScheduledEffect *sprite, s32 type, s32 direction,
                               s32 variant)
{
    u8 *data = gAnimatedBodyPool;
    s32 index = direction * 3 + type * 9;
    u16 *indexPtr = &sprite->animationIndex;
    u32 zero = 0;
    *indexPtr = index;
    sprite->variant = variant;
    sprite->active = 1;
    data = (void *)&((struct SceneScheduledEffectPool *)data)->resource;
    Widget_InitLinked(sprite, data, 13, 0, 1, 2, 0x78, &sprite->position);
    Anim_SetKeyframes(sprite, data, gUnk_086106F0[0x12], 2, zero);
    Anim_Step(sprite, data);
    sprite->animation.flags &= ~1;
}

void SceneBody_Init(struct SceneActorBody *, u32, struct ContactPosition *, u32, u32, void *);

void SceneBody_SetSprite(struct SceneActorBody *, struct ContactSwarmSprite *);

void TerrainOverlay_QueueAddActor(struct SceneActorBody *);

struct SceneScheduledEffect *SceneScheduledEffect_Create(const struct ContactPosition *position,
                                                         u32 id, s32 lifetime, u32 side, u32 type,
                                                         s32 variant, s32 x, s32 y, s32 z,
                                                         s32 delay, u32 flags)
{
    struct SceneScheduledEffect *effect;
    struct SceneActorBody *body;
    struct ContactSwarmSprite *sprite;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    if (!gAnimatedBodyPool)
        return 0;
    effect = SceneScheduledEffect_Allocate(gAnimatedBodyPool);
    if (!effect)
        return 0;
    effect->position = *position;
    effect->lifetime = lifetime;
    effect->timer = 0;
    effect->delay = delay;
    effect->state = 1;
    body = &effect->body;
    SceneBody_Init(body, id, &effect->position, 0, 7, effect);
    sprite = &effect->sprite;
    SceneBody_SetSprite(body, sprite);
    sprite->flags &= ~1;
    effect->flags = flags;
    if (flags & 1) {
        if (side == 0) {
            switch ((s32)type) {
                case 0:
                    body->position.z += 0x20 + gUnk_08610710.z;
                    break;
                case 1:
                    body->position.z += 0x64 + gUnk_08610718.z;
                    break;
                case 2:
                    body->position.z += 0xc8 + gUnk_08610720.z;
                    break;
            }
        } else {
            switch ((s32)type) {
                case 0:
                    body->position.x += 0x20 + gUnk_08610710.z;
                    break;
                case 1:
                    body->position.x += 0x64 + gUnk_08610718.z;
                    break;
                case 2:
                    body->position.x += 0xc8 + gUnk_08610720.z;
                    break;
            }
        }
        TerrainOverlay_QueueAddActor(body);
    }
    StateSprite_InitCollider(effect, side, type, x, y, z);
    StateSprite_InitAnimation(effect, side, type, variant);
    return effect;
}

void SceneScheduledEffect_LoadResource(struct SceneScheduledEffectPool *s)
{
    s->source = ScriptProc_Start(0xcb05, 0x1bc7);
    s->resource = *s->source;
    ResourceHeader_Relocate(&s->resource, s->source);
}

// confidence=low: registers the scheduled effect singleton, loads it and marks its four slots.

s32 SceneScheduledEffect_InitializeManager(struct SceneScheduledEffectPool *p)
{
    s32 i;
    gAnimatedBodyPool = p;
    SceneScheduledEffect_LoadResource(p);
    for (i = 0; i < 4; i++) {
        p->slots[i].slot |= 0xFFFF;
    }
    p->occupied = 0;
    return 0;
}

void SceneScheduledEffect_EnsureSingleton(void)
{
    struct SceneScheduledEffectPool *p;
    if (gAnimatedBodyPool == 0) {
        p = Task_Create(10, 0x550);
        if (p != 0) {
            Task_SetCallbacks(p, (void (*)(void *))StateSpritePool_Tick,
                              (void (*)(void *))AnimatedBodyPool_Destroy);
            if (SceneScheduledEffect_InitializeManager(p) < 0)
                Task_Kill(p);
        }
    }
}

void SceneScheduledEffect_ResetSingleton(void)
{
    gAnimatedBodyPool = 0;
}
