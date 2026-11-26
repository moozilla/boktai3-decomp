#include "engine/shared/contact_position.h"
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "effects/scripted_particle/scene_status_indicator.h"
#include "engine/shared/area_effect_link.h"
#include "engine/shared/engine.h"
#include "engine/shared/scene_vec.h"
#include "engine/shared/engine_globals.h"

/* ScriptedParticle_ResetSingleton: Clears the global pool pointer without removing sprites */

void ScriptedParticle_ResetSingleton(void)
{
    gScriptedParticlePool = 0;
}

/* ScriptedParticle_RunIdleHook: Empty hook beside the pool lifecycle functions; intended role is tentative */

void ScriptedParticle_RunIdleHook(void)
{
}

#define SmallEffectPool_Allocate SceneStatusIndicator_Allocate

struct SceneStatusIndicator *SceneStatusIndicator_Allocate(struct SceneStatusIndicatorManager *pool)
{
    s32 index;
    for (index = 0; index < 12; index++) {
        if (!SceneStatusIndicator_IsAllocated(&pool->allocated, 1, index)) {
            struct SceneStatusIndicator *entry = &pool->entries[index];
            pool->allocated |= 1U << index;
            entry->slotIndex = index;
            return entry;
        }
    }
    return NULL;
}

struct StatusSlot {
    u32 flags;
    u8 pad4[0x24];
    u16 state;
    u8 pad2A[2];
    u16 reset, timer;
};

struct StatusPlayer {
    u8 unknown0[0x448];
    u32 flags;
    u8 unknown44C[0x534 - 0x44C];
    u16 interactionMode;
    u8 unknown536[2];
    u16 passageState;
};

static inline u8 PlayerInMode(struct StatusPlayer *p)
{
    if (p) {
        u32 value = p->passageState;
        u32 active = 0;
        if (value && p->interactionMode == 4)
            active = 1;
        return active;
    }
    return 0;
}

static inline u8 GlobalMode(void)
{
    u32 value = gPlayerPassageState;
    u32 active = 0;
    if (value && gPlayerInteractionMode == 4)
        active = 1;
    return active;
}

static inline u32 TestMask(u32 *p, u32 mask)
{
    return *p & mask;
}

// confidence=low: blinks an indicator for the selected player's active mode.

void SceneStatusIndicator_UpdateBlink(void *owner, struct StatusSlot *input)
{
    struct StatusSlot *p = input;
    struct StatusSlot *sprite = p;
    u32 active = 0;
    if (p->reset)
        p->reset = active;
    if (gEngineFlags & 0x800) {
        s32 local;
        if (gLinkSession && (local = gLinkSession->localPlayer) >= 0) {
            if (!PlayerInMode((struct StatusPlayer *)gPlayers[local]))
                goto checkActive;
            goto show;
        }
    } else {
        if (gWeatherCollisionTimer > 0 ||
            (gPlayers[0] && TestMask(&((struct StatusPlayer *)gPlayers[0])->flags, 16)) ||
            GlobalMode())
            active = 1;
    }
checkActive:
    if (active) {
    show:
        if (!(p->state & 1)) {
            sprite->flags &= ~1;
            p->state |= 1;
        }
        {
            u32 timer = p->timer;
            if (timer) {
                p->timer = timer - 1;
                return;
            }
        }
        {
            u32 flags = p->flags;
            if (flags & 1)
                flags &= ~1;
            else
                flags |= 1;
            p->flags = flags;
        }
    } else {
        sprite->flags |= 1;
        p->state &= 0xFFFE;
    }
    p->timer = 38;
}

#define gEffectPool gSceneStatusIndicatorManager

void Effect_Remove(void *);

void EffectPool_ReleaseSlot(void *unused, struct SceneStatusIndicator *effect)
{
    struct SceneStatusIndicatorManager *pool;
    s32 slot;
    if (effect->reset)
        effect->reset = 0;
    pool = gSceneStatusIndicatorManager;
    slot = effect->slotIndex;
    pool->allocated &= ~(1 << slot);
    effect->active = 0;
    Effect_Remove(effect);
    effect->slotIndex = -1;
    effect->state = 0;
}

#define SceneStatusIndicator_GetOrCreateManager BlinkingEffect_EnsurePool

u8 *BlinkingEffect_EnsurePool(void);

struct SceneStatusIndicator *SceneStatusIndicator_Allocate(struct SceneStatusIndicatorManager *);

void ParticleSprite_Initialize(struct SceneStatusIndicator *, void *, u32);

void Effect_SetOrigin(struct SceneStatusIndicator *, s32, s32);

void SpriteDrawable_SelectTileFrame(struct SceneStatusIndicator *, void *, u32);

struct SceneStatusIndicator *
SceneStatusIndicator_Create(struct SceneStatusIndicatorPosition *position)
{
    struct SceneStatusIndicatorManager *manager = gSceneStatusIndicatorManager;
    struct SceneStatusIndicator *entry;
    if (!manager) {
        manager = (struct SceneStatusIndicatorManager *)BlinkingEffect_EnsurePool();
        if (!manager)
            return 0;
    }
    entry = SceneStatusIndicator_Allocate(manager);
    if (!entry)
        return 0;
    entry->active = 1;
    entry->state = 0;
    entry->blinkTimer = 0;
    entry->reset = 1;
    *(struct SceneStatusIndicatorPosition *)&entry->position = *position;
    entry->position.vector.y += 30;
    ParticleSprite_Initialize(entry, manager->resource, 0);
    Effect_SetOrigin(entry, -8, -8);
    SpriteDrawable_SelectTileFrame(entry, manager->resource, 52);
    entry->flags |= 1;
    entry->mode = 1;
    return entry;
}

extern void *gUnk_08610758[];

static inline u8 allocated(u32 *flags, s32 index)
{
    if (*flags & (1 << index))
        return 1;
    return 0;
}

static inline u32 *SlotMask(struct SceneStatusIndicatorManager *manager)
{
    return &manager->allocated;
}

s32 SceneStatusIndicator_UpdateManager(struct SceneStatusIndicatorManager *manager)
{
    struct SceneStatusIndicator *start = manager->entries;
    struct SceneStatusIndicator *entry;
    s32 i;
    u32 *flags;
    void **callbacks;
    u8 *active;
    if (!manager->allocated)
        return 0;
    i = 0;
    flags = SlotMask(manager);
    callbacks = gUnk_08610758;
    active = &manager->entries[0].active;
    entry = start;
    do {
        if (allocated(flags, i))
            sub_08249240(manager, entry, callbacks[*active]);
        active += sizeof(*entry);
        entry++;
        i++;
    } while (i <= 11);
    return 0;
}

static inline u8 SceneStatusIndicator_TestMask(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}

u32 BlinkingEffect_DestroyPool(struct SceneStatusIndicatorManager *manager)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (SceneStatusIndicator_TestMask(&manager->allocated, i))
            Effect_Remove(&manager->entries[i]);
    }
    gSceneStatusIndicatorManager = 0;
    return 0;
}

s32 BlinkingEffect_InitPool(struct SceneStatusIndicatorManager *manager)
{
    manager->resource = (void *)AnimData_FindEntry(0x1c1a);
    manager->allocated = 0;
    gSceneStatusIndicatorManager = manager;
    return 0;
}

s32 BlinkingEffect_InitPool(struct SceneStatusIndicatorManager *);

s32 SceneStatusIndicator_UpdateManager(struct SceneStatusIndicatorManager *);

u32 BlinkingEffect_DestroyPool(struct SceneStatusIndicatorManager *);

u8 *BlinkingEffect_EnsurePool(void)
{
    u8 *p;
    if (gSceneStatusIndicatorManager != 0)
        return gSceneStatusIndicatorManager;
    p = Task_Create(0xa, 0x260);
    if (p != 0) {
        Task_SetCallbacks(p, SceneStatusIndicator_UpdateManager, BlinkingEffect_DestroyPool);
        if (BlinkingEffect_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void BlinkingEffect_ResetSingleton(void)
{
    gSceneStatusIndicatorManager = 0;
}

/* BlinkingEffect_RunIdleHook: Empty hook beside the effect pool lifecycle functions; intended role is tentative */

void BlinkingEffect_RunIdleHook(void)
{
}

#define BouncingParticle_ShowSprite Sprite_LinkToList

#define BouncingParticle_HideSprite Sprite_UnlinkFromList

struct BurstSpark {
    u32 flags;
    u8 queued, pad5[2], priority;
    u8 pad8[4];
    void *animation;
    u16 frame;
    u8 pad12[0xA];
    struct ContactPosition position;
    u8 pad24[8];
    struct ContactPosition velocity;
    u8 direction, delay;
    u16 stepInterval, age;
    u8 pad3A[2], state, frameShift, slotIndex, allocated;
};

struct BurstSparkPool {
    u8 pad0[0x18];
    u8 animation[0x1C];
    u8 unknown34[4];
    struct BurstSpark entries[128];
};

s32 Mod(s32, s32);

void Sprite_LinkToList(struct BurstSpark *, s32);

void Sprite_UnlinkFromList(struct BurstSpark *, u32);

void BouncingParticle_Update(void *unused, struct BurstSpark *particle)
{
    struct BurstSpark *sprite = particle;
    s32 frame;
    if ((s8)particle->delay > 0) {
        particle->delay--;
    } else {
        if ((s8)particle->delay == 0)
            particle->delay |= 0xFF;
        particle->age++;
        frame = particle->age >> particle->frameShift;
        if (frame == 6) {
            particle->state = 2;
        } else if (Mod(particle->age, particle->stepInterval)) {
            if (particle->queued)
                Sprite_UnlinkFromList(particle, 0);
        } else {
            if (!particle->queued)
                Sprite_LinkToList(particle, 0);
            sprite->position.x += sprite->velocity.x;
            sprite->position.y += sprite->velocity.y;
            sprite->position.z += sprite->velocity.z;
            sprite->velocity.x = (s16)sprite->velocity.x >> 1;
            sprite->velocity.z = (s16)sprite->velocity.z >> 1;
            sprite->frame = frame;
        }
    }
}

void BurstSpark_ReleaseSlot(u32 unused, struct BurstSpark *p)
{
    if (p->queued)
        Sprite_Remove(p);
    p->state = 0;
    Memory_Clear(p, sizeof(*p));
}

struct BurstSpark *BurstSpark_AllocateSlot(struct BurstSparkPool *pool)
{
    s32 i;
    for (i = 0; i < 128; i++) {
        struct BurstSpark *entry = &pool->entries[i];
        if (entry->allocated != 1) {
            entry->slotIndex = i;
            entry->allocated = 1;
            return entry;
        }
    }
    return NULL;
}

extern const s16 gUnk_086149C4[];

u8 *BurstSpark_EnsurePool(void);

void Sprite_InitUnlinked(void *, void *, s32);

static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline u32 LastRandom(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    {
        u32 offset = index * 2;
        const u16 *lastTable = (const u16 *)RANDOM_TABLE_ADDR;
        return *(const u16 *)(offset + (u32)lastTable);
    }
}

static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline s32 Scaled(s32 value, s32 speed)
{
    s32 sample = value;
    s32 magnitude = speed;
    return Truncate(sample * magnitude);
}

// confidence=low: creates a randomized radial spark with optional palette selection.

s32 BurstSpark_Create(const struct ContactPosition *position, s32 priority, s32 delay,
                      s32 direction, s32 speed, s32 mode, s32 palette)
{
    struct BurstSparkPool *pool;
    struct BurstSpark *p, *sprite;
    const u16 *table;
    struct ContactPosition *velocity;
    if (!position)
        return 0;
    pool = gBurstSparkPool;
    if (!pool) {
        pool = (struct BurstSparkPool *)BurstSpark_EnsurePool();
        if (!pool)
            return 0;
    }
    p = BurstSpark_AllocateSlot(pool);
    if (!p)
        return 0;
    sprite = p;
    Sprite_InitUnlinked(sprite, pool->animation, 0);
    sprite->frame = 0;
    sprite->flags &= ~1;
    sprite->priority = priority;
    sprite->position = *position;
    sprite->state = 1;
    sprite->age = 0;
    sprite->direction = direction;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    sprite->delay = (RandomValue(table) & 7) + delay;
    sprite->stepInterval = (RandomValue(table) & 7) + 1;
    velocity = &sprite->velocity;
    velocity->x = Scaled(gUnk_086149C4[(direction + 64) & 255], speed);
    velocity->y = 0;
    velocity->z = Scaled(gUnk_086149C4[direction & 255], speed);
    p->velocity.y = LastRandom() & 31;
    p->frameShift = mode;
    {
        s32 test = palette;
        if (test > 0) {
            void *anim = sprite->animation;
            s32 bank = palette;
            AnimData_SelectPaletteBank(anim, bank + 0x153);
        }
    }
    return 0;
}

#define gEffectPoolUpdateCallbacks gUnk_08610764

extern void *gUnk_08610764[];

s32 sub_08249240(void *, void *, void *);

s32 EffectPool_UpdateActive(struct BurstSparkPool *pool)
{
    void **callbacks = gUnk_08610764;
    struct BurstSpark *effect = pool->entries;
    u8 *state = &effect->state;
    s32 i = 127;
    do {
        if (state[3] == 1)
            sub_08249240(pool, effect, callbacks[state[0]]);
        effect++;
        state += 0x40;
    } while (--i >= 0);
    return 0;
}

u32 BurstSpark_DestroyPool(struct BurstSparkPool *pool)
{
    struct BurstSpark *entry = pool->entries;
    u8 *allocated = &entry->allocated;
    s32 i = 127;
    u32 z;
    do {
        if (*allocated == 1 && entry->queued)
            Sprite_Remove(entry);
        entry++;
        allocated += sizeof(*entry);
    } while (--i >= 0);
    z = 0;
    gBurstSparkPool = z;
    return z;
}

s32 BurstSpark_InitPool(u8 *p)
{
    AnimData_Load(p + 0x18, 0x8639);
    gBurstSparkPool = p;
    return 0;
}

s32 BurstSpark_InitPool(u8 *);

s32 EffectPool_UpdateActive(struct BurstSparkPool *);

u32 BurstSpark_DestroyPool(struct BurstSparkPool *);

u8 *BurstSpark_EnsurePool(void)
{
    u8 *p;
    if (gBurstSparkPool != 0)
        return gBurstSparkPool;
    p = Task_Create(0xa, 0x2038);
    if (p != 0) {
        Task_SetCallbacks(p, EffectPool_UpdateActive, BurstSpark_DestroyPool);
        if (BurstSpark_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void BurstSpark_ResetSingleton(void)
{
    gBurstSparkPool = 0;
}
