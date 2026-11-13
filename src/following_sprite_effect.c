#include "following_sprite_effect.h"

void ParticleBurstEffect_ResetManager(void)
{
    gSceneProcParticles = 0;
}

void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *effect,
                                            void (*update)(struct FollowingEffect *))
{
    effect->update = update;
    effect->entered = 1;
}

void FollowingSpriteEffect_OnStop(struct FollowingEffect *effect)
{
    u8 *entered = &effect->entered;
    if (*entered != 0) {
        u8 *active;
        u32 zero;
        effect->sprite.flags |= 1;
        active = &effect->active;
        zero = 0;
        *active = zero;
        effect->update = 0;
        *entered = zero;
    }
}

s32 Div(s32, s32);

void Collider_SetFixedPositionWithYOffset(void *, struct FollowingPosition *, s32);

void SpatialIndex_InsertEntry(void *);

void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *,
                                            void (*)(struct FollowingEffect *));

void FollowingSpriteEffect_OnStop(struct FollowingEffect *);

/* Tentative: ease toward the anchor while playing the attached sprite. */

void FollowingSpriteEffect_UpdateFollowing(struct FollowingEffect *p)
{
    struct FollowingPosition target;
    s32 delta, height, pixels;
    struct FollowingPosition *pos;
    struct ContactPosition *dst;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->position = *p->anchor;
        p->entered = 0;
    }
    p->position.y = p->anchor->y;
    p->position.z = p->anchor->z + 0x12000;
    target.x = p->anchor->x + 0x40000;
    delta = Div(target.x - p->position.x, 8);
    pixels = delta >> 12;
    pos = &p->position;
    if (pixels > 6)
        delta = Div(target.x - pos->x, 4);
    else if (pixels > 4)
        delta = Div(target.x - pos->x, 6);
    pos->x += delta;
    height = *p->height - 0x20000;
    dst = &p->sprite.position;
    dst->x = (pos->x >> 12) - gSceneCamera->x;
    dst->y = ((s32)(pos->z + (pos->y + height)) >> 12) + 0x60;
    dst->z = pos->z >> 12;
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, pos, height);
    SpatialIndex_InsertEntry(p->collider);
    if (!--p->duration)
        FollowingSpriteEffect_SetStateCallback(p, FollowingSpriteEffect_OnStop);
}

s32 PairCallbackPool_Update(struct FollowingSpriteManager *p)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        if (p->entries[i].active) {
            void *callback = p->entries[i].update;
            if (callback)
                sub_08249240(&p->entries[i], i * sizeof(p->entries[0]), callback);
        }
    }
    p->frame++;
    return 0;
}

u32 FollowingSpriteEffect_DestroyManager(void)
{
    s32 i = 0;
    do {
        SpritePair_ReleaseSlot(i, 1);
        i++;
    } while (i <= 1);
    return gSpritePairPool = 0;
}

s32 SpritePairPool_Initialize(struct FollowingSpriteManager *pool)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        struct FollowingEffect *entry = &pool->entries[i];
        if (!AnimData_Load(entry->resource, 0x409c))
            return -1;
        Sprite_Init(&entry->sprite, entry->resource, 0);
        entry->sequence = ScriptProc_Start(0x922e, 0x1c22);
        Anim_SetSequence(&entry->anim, entry->sequence, 9, 0, 0);
        AnimData_SelectPaletteBank(entry->resource, 0xe4);
        Memory_Clear(&entry->sprite.position, 8);
        entry->sprite.flags |= 0x11;
        entry->sprite.priority = 1;
        entry->active = 0;
    }
    pool->frame = 0;
    gSpritePairPool = pool;
    return 0;
}

struct FollowingSpriteManager *FollowingSpriteEffect_EnsureManager(void)
{
    struct FollowingSpriteManager *p;
    if (gSpritePairPool != 0)
        return gSpritePairPool;
    p = Task_Create(0xa, 0x1d4);
    if (p != 0) {
        Task_SetCallbacks(p, PairCallbackPool_Update, FollowingSpriteEffect_DestroyManager);
        if (SpritePairPool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 PairCallbackPool_MarkNextOfKind(struct FollowingSpriteManager *p, s32 first, s32 kind,
                                    u8 *visited)
{
    s32 i;
    for (i = first; i < 2; i++) {
        if (p->entries[i].active && !visited[i] && p->entries[i].id == kind) {
            visited[i] = 1;
            return 1;
        }
    }
    return 0;
}

u8 SpritePairPool_TestUnavailable(struct FollowingSpriteManager *pool, u32 id, s32 required)
{
    s32 matches = 0;
    s32 available = 0;
    s32 i;
    u32 key;
    u8 seen[2];
    key = id & 0xff;
    Memory_Clear(seen, 2);
    for (i = 0; i < 2; i++) {
        if (!pool->entries[i].active)
            available++;
        else if ((pool->entries[i].id & 0xff) == key && !seen[i]) {
            seen[i] = 1;
            PairCallbackPool_MarkNextOfKind(pool, i + 1, pool->entries[i].id, seen);
            matches++;
        }
    }
    if (matches <= 0 && available >= required)
        return 0;
    return 1;
}

void FollowingSpriteEffect_OnAnimationComplete(void)
{
}

void Position_GetHeightAdjustedCoordinates(void *, u32, void *);

void SpatialIndex_InitEntry(void *, void *, struct FollowingEffectBounds *,
                            struct FollowingEffectBounds *, u32, u32, u32, u32, u32, void (*)(void),
                            void *);

void SpritePairEntry_CreateEffect(struct FollowingEffect *entry, void (*callback)(void), u32 kind,
                                  u32 mode)
{
    u32 position[2];
    struct FollowingEffectBounds first, second;
    u32 height = *entry->height - 0x20000;
    Position_GetHeightAdjustedCoordinates(entry->anchor, height, position);
    first.horizontal.low = 10;
    first.horizontal.high = 8;
    first.vertical.low = 23;
    second.horizontal.low = 10;
    second.horizontal.high = first.horizontal.high;
    second.vertical.low = -21;
    SpatialIndex_InitEntry(entry->collider, position, &first, &second, 12, mode, 5, kind, 20,
                           callback, entry);
}

static inline u32 SpritePairPool_MakeKey(u32 counter, u32 id)
{
    u32 mask = 0xff;
    id &= mask;
    counter &= mask;
    counter <<= 8;
    id |= counter;
    return id;
}

s32 SpritePairPool_StartEntry(struct FollowingPosition *position, s32 *height, u32 unused,
                              struct FollowingSpriteParameters *params, u32 id)
{
    struct FollowingSpriteManager *pool;
    s32 i;
    u32 key;
    if (!gSpritePairPool) {
        pool = FollowingSpriteEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gSpritePairPool;
    key = pool->frame;
    key = SpritePairPool_MakeKey(key, id);
    if (SpritePairPool_TestUnavailable(pool, key, 1))
        return -1;
    for (i = 0; i < 2; i++) {
        if (!pool->entries[i].active) {
            pool->entries[i].anchor = position;
            pool->entries[i].height = height;
            pool->entries[i].duration = params->duration;
            pool->entries[i].active = 1;
            key = pool->frame;
            key = SpritePairPool_MakeKey(key, id);
            pool->entries[i].id = key;
            SpritePairEntry_CreateEffect(&pool->entries[i],
                                         FollowingSpriteEffect_OnAnimationComplete, params->value,
                                         id + 1);
            FollowingSpriteEffect_SetStateCallback(&pool->entries[i],
                                                   FollowingSpriteEffect_UpdateFollowing);
            Sound_PlaySong(0x3e4);
            return i;
        }
    }
    return -1;
}

void SpritePair_ReleaseSlot(u32 index, u32 remove)
{
    struct FollowingSpriteManager *pair = gSpritePairPool;
    if (pair && index <= 1 && pair->entries[index].sprite.unknown4[0]) {
        pair->entries[index].active = 0;
        pair->entries[index].id = 0;
        pair->entries[index].update = 0;
        if (remove == 1)
            Sprite_Remove(&pair->entries[index].sprite);
        else
            pair->entries[index].sprite.flags |= 1;
    }
}

void FollowingSpriteEffect_ResetManager(void)
{
    gSpritePairPool = 0;
}

s32 SpritePair_UpdateAnimation(struct PositionLinkedSpriteManager *m)
{
    s32 i = 0;
    for (; i < 2; i++) {
        struct PositionLinkedSprite *e = &m->entries[i];
        if (!e->active)
            continue;
        e->sprite.position.x = (e->position->x >> 12) - (s16)(gSceneCamera->x - 16);
        e->sprite.position.y = ((e->position->z + e->position->y + *e->height) >> 12) + 0x4e;
        PlayerCue_StepSequence(&e->sprite, &e->animation);
    }
    return 0;
}

s32 PositionLinkedSprite_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 1; i++)
        SpritePair_RemoveSlot(i);
    return gCompactSpritePool = 0;
}

s32 CompactSpritePool_Initialize(struct PositionLinkedSpriteManager *pool)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        struct PositionLinkedSprite *entry = &pool->entries[i];
        if (!AnimData_Load(entry->resource, 0x409c))
            return -1;
        Sprite_Init(&entry->sprite, entry->resource, 0);
        AnimData_SelectPaletteBank(entry->resource, 0xe4);
        pool->sequence = ScriptProc_Start(0x922e, 0x1c22);
        Anim_SetSequence(&entry->animation, pool->sequence, 6, 0, 0);
        Memory_Clear(&entry->sprite.position, 8);
        pool->entries[i].active = 0;
        pool->entries[i].sprite.flags |= 0x11;
        pool->entries[i].sprite.priority = 1;
    }
    gCompactSpritePool = pool;
    return 0;
}

struct PositionLinkedSpriteManager *PositionLinkedSprite_EnsureManager(void)
{
    struct PositionLinkedSpriteManager *p;
    if (gCompactSpritePool != 0)
        return gCompactSpritePool;
    p = Task_Create(0xa, 0xe4);
    if (p != 0) {
        Task_SetCallbacks(p, SpritePair_UpdateAnimation, PositionLinkedSprite_DestroyManager);
        if (CompactSpritePool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 DisplaySlots_Reserve(struct FollowingPosition *position, s32 *height)
{
    struct PositionLinkedSpriteManager *p;
    s32 i;
    if (!gCompactSpritePool) {
        p = PositionLinkedSprite_EnsureManager();
        if (!p)
            return -1;
    } else
        p = gCompactSpritePool;
    for (i = 0; i < 2; i++) {
        if (p->entries[i].active == 0) {
            p->entries[i].position = position;
            p->entries[i].sprite.flags |= 1;
            p->entries[i].height = height;
            p->entries[i].active = 1;
            return i;
        }
    }
    return -1;
}

void SpritePair_RemoveSlot(u32 index)
{
    struct PositionLinkedSpriteManager *pair = gCompactSpritePool;
    if (pair && index <= 1 && pair->entries[index].sprite.unknown4[0]) {
        Sprite_Remove(&pair->entries[index].sprite);
        pair->entries[index].active = 0;
    }
}

void ActorMarker_ResetManager(void)
{
    gCompactSpritePool = 0;
}

void ActorMarker_HideSlot(s32 slot)
{
    struct PositionLinkedSpriteManager *manager;
    if (slot >= 0) {
        manager = (void *)gCompactSpritePool;
        if (manager != 0 && manager->entries[slot].active != 0)
            manager->entries[slot].sprite.flags |= 1;
    }
}

void ActorMarker_ShowSlot(s32 slot)
{
    struct PositionLinkedSpriteManager *manager;
    if (slot >= 0) {
        manager = (void *)gCompactSpritePool;
        if (manager != 0 && manager->entries[slot].active != 0)
            manager->entries[slot].sprite.flags &= ~1;
    }
}

void SpriteParticle_InitSystem(void)
{
}

void SpriteDrawable_SelectTileFrame(struct ScreenMoteSprite *, void *, u32);

void ScreenMote_ApproachTarget(struct ScreenMoteManager *m, struct ScreenMoteState *s)
{
    struct ScreenMoteSprite *sprite = &m->sprites[s->control.spriteIndex];
    union ScreenMotePosition target;
    if (s->control.entering == 1) {
        sprite->flags &= ~1;
        s->control.remaining = 12;
        s->control.delay = 0;
        s->control.entering = 0;
    }
    target = *s->target;
    target.axis.x += 16;
    target.axis.y -= 12;
    s->position.axis.x += Div(target.axis.x - s->position.axis.x, s->control.remaining);
    s->position.axis.y += Div(target.axis.y - s->position.axis.y, s->control.remaining);
    s->control.remaining--;
    sprite->position = s->position;
    if (++s->control.delay > 3)
        SpriteDrawable_SelectTileFrame(sprite, m->resource, 3);
    if (s->control.remaining == 0)
        s->control.phase = 2;
    s->control.age++;
}

void SpriteParticle_ReleaseSlot(struct ScreenMoteManager *manager, struct ScreenMoteState *state)
{
    u32 zero;
    Effect_Remove(&manager->sprites[state->control.spriteIndex]);
    zero = 0;
    state->control.phase = zero;
    state->control.active = zero;
    Memory_Clear(state, sizeof(*state));
    state->control.spriteIndex = -1;
}

struct ScreenMoteState *SpriteParticle_AllocateSlot(struct ScreenMoteManager *manager)
{
    s32 slot = 0;
    u32 one = 1;
    u32 *active = &manager->states[0].control.active;
    struct ScreenMoteState *state = manager->states;
    do {
        if (*active == 1) {
            active = (void *)((u8 *)active + sizeof(*state));
            state++;
            slot++;
            continue;
        }
        state->control.spriteIndex = slot;
        *active = one;
        return state;
    } while (slot <= 31);
    return 0;
}

extern const u16 gUnk_086141BC[];

s32 Mod(s32, s32);

void ParticleSprite_Initialize(struct ScreenMoteSprite *, void *, u32);

void Effect_SetOrigin(struct ScreenMoteSprite *, s32, s32);

void Effect_SetPalette(struct ScreenMoteSprite *, u32);

static inline u32 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

s32 ScreenMote_SpawnBatch(union ScreenMotePosition *origin, union ScreenMotePosition *target,
                          s16 *range, s32 count, u32 mode, u32 unused, u32 flags, u32 options)
{
    struct ScreenMoteManager *m;
    struct ScreenMoteState *s;
    struct ScreenMoteSprite *sprite;
    s32 i;
    if (!origin)
        return -1;
    m = gScreenMoteManager;
    if (!m) {
        m = SpriteParticle_GetOrCreateManager();
        if (!m)
            return -1;
    }
    for (i = 0; i < count; i++) {
        s = SpriteParticle_AllocateSlot(m);
        if (!s)
            return -1;
        sprite = &m->sprites[s->control.spriteIndex];
        s->position = *origin;
        s->target = target;
        s->control.entering = 1;
        if (range) {
            if (range[0])
                s->position.axis.x += Mod(NextRandom(), range[0]);
            if (range[1])
                s->position.axis.y += Mod(NextRandom(), range[1] * 2) - range[1];
        }
        ParticleSprite_Initialize(sprite, m->resource, 0);
        Effect_SetOrigin(sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(sprite, m->resource, 2);
        sprite->flags |= 0x11 | flags;
        sprite->mode = mode;
        Effect_SetPalette(sprite, 1);
        s->control.phase = 1;
        s->control.options = options;
    }
    return 0;
}

s32 SpriteParticle_SpawnSingleFrameBatch(union ScreenMotePosition *origin,
                                         union ScreenMotePosition *target, s16 *range, s32 count,
                                         u32 mode, u32 flags, u32 unused)
{
    return ScreenMote_SpawnBatch(origin, target, range, count, mode, unused, flags, 1);
}

s32 SpriteParticle_SpawnAnimatedBatch(union ScreenMotePosition *origin,
                                      union ScreenMotePosition *target, s16 *range, s32 count,
                                      u32 mode, u32 flags, u32 unused)
{
    return ScreenMote_SpawnBatch(origin, target, range, count, mode, unused, flags, 0);
}

extern void *gUnk_086108EC[];

s32 ScreenMote_UpdateManager(struct ScreenMoteManager *manager)
{
    s32 remaining;
    u32 pauseMask = 0x200;
    struct ScreenMoteControl *control = &manager->states[0].control;
    struct ScreenMoteState *state = manager->states;
    remaining = 31;
    do {
        if (control->active) {
            if (((gEngineFlags | gScenePauseFlags) & pauseMask) ||
                ((gEngineFlags & 0x800) && (gEnginePauseFlags & 7))) {
                if (control->options & 1)
                    goto next;
            }
            sub_08249240(manager, state, gUnk_086108EC[control->phase]);
        }
    next:
        control = (void *)((u8 *)control + sizeof(*state));
        state++;
        remaining--;
    } while (remaining >= 0);
    return 0;
}

s32 EffectPool_Destroy(struct ScreenMoteManager *pool)
{
    struct ScreenMoteState *entry = pool->states;
    s32 count = 31;
    do {
        if (entry->control.active)
            Effect_Remove(&pool->sprites[entry->control.spriteIndex]);
        entry++;
        count--;
    } while (count >= 0);
    {
        struct ScreenMoteManager *cleared = 0;
        gScreenMoteManager = cleared;
    }
    return 0;
}

void SpriteParticle_LoadAnimationData(struct ScreenMoteManager *manager)
{
    manager->resource = AnimData_FindEntry(0x1c1e);
}

s32 SpriteParticle_InitManager(struct ScreenMoteManager *manager)
{
    SpriteParticle_LoadAnimationData(manager);
    gScreenMoteManager = (u32)manager;
    return 0;
}

struct ScreenMoteManager *SpriteParticle_GetOrCreateManager(void)
{
    struct ScreenMoteManager *p;
    if (gScreenMoteManager != 0)
        return gScreenMoteManager;
    p = Task_Create(0xa, 0x99c);
    if (p != 0) {
        Task_SetCallbacks(p, ScreenMote_UpdateManager, EffectPool_Destroy);
        if (SpriteParticle_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
