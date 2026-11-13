#include "sprite_effect.h"

void SceneSpriteBurst_Shutdown(void)
{
}

void SceneActor14_OnColliderContact(void)
{
}

void SpriteEffect_InitCollider(struct SpriteEffect *effect, s32 damage, s32 knockback, s32 element,
                               s32 extra)
{
    union SpriteEffectVector size, offset;
    void *c;
    size.packed.x = 16;
    size.packed.y = 16;
    size.packed.z = 16;
    offset.packed.x = 0;
    offset.packed.y = 0;
    offset.packed.z = 0;
    c = &effect->collider;
    Collider_Init(c, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetAttackProperties(c, damage, knockback, 0, element, extra);
    Collider_SetPosition(c, &effect->position, 0);
    Collider_SetCallback(c, SceneActor14_OnColliderContact, effect);
}

static inline u8 IsUsed(struct SpriteEffectPool *pool, u32 index)
{
    if (pool->used & (1U << index))
        return 1;
    return 0;
}

struct SpriteEffect *SpriteEffectPool_AllocateSlot(struct SpriteEffectPool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!IsUsed(pool, i)) {
            struct SpriteEffect *slot = &pool->effects[i];
            pool->used |= 1U << i;
            slot->slot = i;
            return slot;
        }
    }
    return 0;
}

static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 groundHeight(struct SceneVec *position)
{
    s32 index = cellIndex(position);
    u32 height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

void SpriteEffect_UpdateFallingShard(struct SpriteEffectPool *manager, struct SpriteEffect *effect)
{
    struct SceneVec low, high;
    s32 y;
    struct SpriteEffect *self = effect;
    effect->previous = effect->position;
    effect->flags &= ~1;
    effect->position.coordinates.x += effect->motion.coordinates.x;
    effect->position.coordinates.y += effect->motion.coordinates.y;
    effect->position.coordinates.z += effect->motion.coordinates.z;
    effect->motion.coordinates.y--;
    y = effect->previous.coordinates.y;
    if (y < groundHeight(&effect->previous.coordinates) || self->previous.coordinates.y <= 0) {
        Sound_PlaySong(0x1d6);
        manager->used &= ~(1 << effect->slot);
        effect->active = 0;
        Sprite_Remove(self);
        Shadow_Destroy(effect->shadow);
        effect->slot = -1;
        effect->state = 0;
        effect->timer = 0;
        low.x = -12;
        {
            u32 table = (u32)gRandomTable;
            u32 index = (gRandomIndex + 1) & 1023;
            low.y = HitSpark_Sample(table, index) & 7;
            low.z = -12;
            high.x = 24;
            index = HitSpark_NextIndex(index, 1023);
            gRandomIndex = index;
            high.y = (HitSpark_Sample(table, index) & 7) + 3;
            high.z = 24;
        }
        sub_08014634(6, 3, &effect->position, &low, &high, 32, 31);
    } else {
        Collider_SetPosition(&effect->collider, &effect->position, 0);
        Collider_Refresh(&effect->collider);
    }
}

void SpriteEffect_UpdateBlinkAndExpire(struct SpriteEffectPool *owner, struct SpriteEffect *effect)
{
    struct SpriteEffect *p = effect;
    s32 rate;
    if (effect->timer > 60)
        rate = 2;
    else if (effect->timer > 40)
        rate = 1;
    else
        rate = 0;
    if (Mod(effect->timer, gUnk_086106E0[rate]) == 0) {
        if (p->flags & 1)
            p->flags &= ~1;
        else
            p->flags |= 1;
    }
    if (effect->timer > 90) {
        owner->used &= ~(1 << effect->slot);
        effect->active = 0;
        Sprite_Remove(p);
        Shadow_Destroy(effect->shadow);
        effect->slot = -1;
        effect->state = 0;
        effect->timer = 0;
    } else {
        effect->timer++;
    }
}

void SceneActor14_GetFacingAndFlip(struct SpriteEffectSource *source, u8 *variant, u8 *flip)
{
    u32 quadrant = ((source->facing + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *variant = 3 - quadrant;
    } else {
        *flip = 0;
        *variant = quadrant;
    }
}

s32 SpriteEffect_Spawn(struct SpriteEffectSource *source, struct SpriteEffectSource *target,
                       union SpriteEffectVector *motion, u32 damage, u32 knockback, u32 element,
                       u32 extra)
{
    return SpriteEffect_SpawnWithMotion(source, target, motion, damage, knockback, element, extra);
}

s32 SpriteEffect_SpawnTowardObject(struct SpriteEffectSource *source,
                                   struct SpriteEffectSource *target, u32 damage, u32 knockback,
                                   u32 element, u32 extra)
{
    s32 k2 = 1023;
    union SpriteEffectVector motion;
    s32 duration;
    const u16 *tbl;

    motion.packed.x = target->position.packed.x - source->position.packed.x;
    motion.packed.z = target->position.packed.z - source->position.packed.z;
    tbl = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & k2;
    {
        u32 v = tbl[gRandomIndex];
        u32 m = 63;
        duration = (v & m) + 30;
    }
    motion.packed.x = Div(motion.packed.x, duration);
    motion.packed.z = Div(motion.packed.z, duration);
    {
        s32 q = Div(duration, 2);
        u32 v = tbl[gRandomIndex = (gRandomIndex + 1) & 1023];
        u32 m = 7;
        motion.packed.y = q + (v & m);
    }
    if (motion.packed.x > 48) {
        motion.packed.x = 48;
    }
    if (motion.packed.x < (-48)) {
        motion.packed.x = -48;
    }
    if (motion.packed.z > 48) {
        motion.packed.z = 48;
    }
    if (motion.packed.z < (-48)) {
        motion.packed.z = -48;
    }
    return SpriteEffect_SpawnWithMotion(source, target, &motion, damage, knockback, element, extra);
}

s32 SpriteEffect_SpawnWithMotion(struct SpriteEffectSource *source,
                                 struct SpriteEffectSource *target,
                                 union SpriteEffectVector *motion, u32 damage, u32 knockback,
                                 u32 element, u32 extra)
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
    effect->previous = effect->position;
    Sprite_Init(effect, pool->spriteData, 0);
    effect->frame = 3;
    AnimData_SelectPaletteBank(effect->animation, 0x2c4);
    effect->flags |= 1;
    effect->layer = 2;
    SpriteEffect_InitCollider(effect, damage, knockback, element, extra);
    return effect->slot;
}

s32 SpriteEffectPool_Update(struct SpriteEffectPool *pool)
{
    struct SpriteEffect *slots = pool->effects;
    if (pool->used) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (IsUsed(pool, i)) {
                struct SpriteEffect *s = (void *)(i * sizeof(*slots) + (u32)slots);
                sub_08249240(pool, s, gUnk_086106E4[s->active]);
            }
        }
    }
    return 0;
}

s32 SpriteEffectPool_RemoveSlots(struct SpriteEffectPool *pool)
{
    s32 i = 0;
    for (; i < 8; i++) {
        if (IsUsed(pool, i)) {
            struct SpriteEffect *slot = &pool->effects[i];
            Sprite_Remove(slot);
            Shadow_Destroy(slot->shadow);
        }
    }
    return (s32)(gSpriteEffectPool = 0);
}

s32 SceneActor14_InitManager(struct SpriteEffectPool *pool)
{
    pool->used = 0;
    gSpriteEffectPool = pool;
    AnimData_Load(pool->spriteData, 0x84EE);
    return 0;
}

struct SpriteEffectPool *SceneActor14_EnsureSingleton(void)
{
    struct SpriteEffectPool *pool;
    if (gSpriteEffectPool != 0)
        return gSpriteEffectPool;
    pool = Task_Create(0xa, 0x6f8);
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))SpriteEffectPool_Update,
                          (void (*)(void *))SpriteEffectPool_RemoveSlots);
        if (SceneActor14_InitManager(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

void SceneActor14_ResetSingleton(void)
{
    gSpriteEffectPool = 0;
}

void SceneActor14_Shutdown(void)
{
}

s32 SceneActor14_UpdateAnimState(struct SceneActor14 *actor)
{
    union {
        struct SceneActor14Manager *manager;
        void *animation;
    } data;
    s32 idx;
    data.manager = gAnimatedBodyPool;

    if (actor->animationFlags & 1) {
        if (actor->state == -1 && actor->frame > actor->frameLimit) {
            Anim_SetKeyframes(actor, data.manager->animation, gUnk_086106F0[0x12], 2, 0);
            return 0;
        }
        if (actor->state == 0 && !(gEngineFlags & 0x800))
            Sound_PlaySong(0x1A8);
        if (actor->state == 1 && actor->counter > 0) {
            data.animation = data.manager->animation;
            Anim_SetKeyframes(actor, data.animation, gUnk_086106F0[actor->base + actor->state], 2,
                              0);
            Anim_Step(actor, data.animation);
            return 0;
        }
        actor->state += actor->step;
        idx = actor->base + actor->state;
        if (actor->state == 3) {
            s32 b = actor->slot;
            data.manager->used &= ~(1 << b);
            actor->flags |= 1;
            Anim_Remove(actor);
            if (actor->bodyFlags & 1)
                TerrainOverlay_QueueRemoveActor(actor->body);
            SceneBody_Unregister(actor->body);
            actor->slot = -1;
            actor->phase = 2;
            return 1;
        }
        data.animation = data.manager->animation;
        Anim_SetKeyframes(actor, data.animation, gUnk_086106F0[idx], 2, 0);
        Anim_Step(actor, data.animation);
    } else {
        Anim_Step(actor, data.manager->animation);
    }
    if (actor->state == 1)
        actor->counter--;
    return 0;
}
