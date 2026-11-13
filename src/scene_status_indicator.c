#include "global.h"
#include "ram.h"
#include "scene_status_indicator.h"
#include "area_effect_link.h"
#include "engine.h"
#include "scene_vec.h"
#include "engine_globals.h"


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
    s32 index = 0;
    volatile u32 *allocated = &pool->allocated;
    u16 one = 1;
    u8 *kind = &pool->entries[0].slotIndex;
    struct SceneStatusIndicator *entry = pool->entries;
    do {
        if (SceneStatusIndicator_IsAllocated(allocated, one, index)) {
            kind += 0x30;
            entry++;
            index++;
        } else {
            u32 bit = one;
            (*(&bit)) <<= index;
            *allocated |= bit;
            *kind = index;
            return entry;
        }
    } while (index <= 11);
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
    u32 one;
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
    one = 1;
    entry->reset = one;
    *(struct SceneStatusIndicatorPosition *)&entry->position = *position;
    entry->position.vector.y += 30;
    ParticleSprite_Initialize(entry, manager->resource, 0);
    Effect_SetOrigin(entry, -8, -8);
    SpriteDrawable_SelectTileFrame(entry, manager->resource, 52);
    entry->flags |= 1;
    entry->mode = one;
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
    s32 i = 0;
    u32 *allocated = &manager->allocated;
    struct SceneStatusIndicator *entry = manager->entries;
    for (; i < 12; entry++, i++) {
        if (SceneStatusIndicator_TestMask(allocated, i))
            Effect_Remove(entry);
    }
    {
        u32 z = 0;
        gSceneStatusIndicatorManager = z;
    }
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

struct BouncingParticle {
    u32 flags;
    u8 queued, unknown5[11];
    u16 frame;
    u8 unknown12[10];
    u16 x, y, z;
    u8 unknown22[10];
    u16 velocityX, velocityY, velocityZ, unknown32;
    u8 unknown34;
    u8 delay;
    u16 stepInterval, age;
    u8 unknown3A[2], state, frameShift;
};

s32 Mod(s32, s32);

void Sprite_LinkToList(struct BouncingParticle *, s32);

void Sprite_UnlinkFromList(struct BouncingParticle *, u32);

void BouncingParticle_Update(void *unused, struct BouncingParticle *particle)
{
    struct BouncingParticle *sprite = particle;
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
            sprite->x += sprite->velocityX;
            sprite->y += sprite->velocityY;
            sprite->z += sprite->velocityZ;
            sprite->velocityX = (s16)sprite->velocityX >> 1;
            sprite->velocityZ = (s16)sprite->velocityZ >> 1;
            sprite->frame = frame;
        }
    }
}

void BurstSpark_ReleaseSlot(u32 a, u8 *p)
{
    if (p[4] != 0)
        Sprite_Remove(p);
    p[0x3c] = 0;
    Memory_Clear(p, 0x40);
}

u8 *BurstSpark_AllocateSlot(u8 *p)
{
    s32 i = 0;
    u32 one = 1;
    u8 *res;
    u8 *r = p + 0x76;
    u8 *q = p + 0x38;
    do {
        if (r[1] == 1) {
            r += 0x40;
            q += 0x40;
            i++;
            continue;
        }
        r[0] = i;
        r[1] = one;
        res = q;
        goto end;
    } while (i <= 0x7f);
    res = 0;
end:
    return res;
}

struct SparkPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

struct BurstSpark {
    u32 flags;
    u8 pad4[3], priority;
    u8 pad8[4];
    void *animation;
    u16 frame;
    u8 pad12[0xA];
    struct SparkPosition position;
    u8 pad24[8];
    struct SparkPosition velocity;
    u8 direction, delay;
    u16 speed, timer;
    u8 pad3A[2], active, mode, pad3E[2];
};

struct BurstSparkPool {
    u8 pad0[0x18];
    u8 animation[0x1C];
};

extern const s16 gUnk_086149C4[];

u8 *BurstSpark_EnsurePool(void);

u8 *BurstSpark_AllocateSlot(u8 *);

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


s32 BurstSpark_Create(const struct SparkPosition *position, s32 priority, s32 delay, s32 direction,
                      s32 speed, s32 mode, s32 palette)
{
    struct BurstSparkPool *pool;
    struct BurstSpark *p, *sprite;
    const u16 *table;
    struct SparkPosition *velocity;
    if (!position)
        return 0;
    pool = gBurstSparkPool;
    if (!pool) {
        pool = (struct BurstSparkPool *)BurstSpark_EnsurePool();
        if (!pool)
            return 0;
    }
    p = (struct BurstSpark *)BurstSpark_AllocateSlot((u8 *)pool);
    if (!p)
        return 0;
    sprite = p;
    Sprite_InitUnlinked(sprite, pool->animation, 0);
    sprite->frame = 0;
    sprite->flags &= ~1;
    sprite->priority = priority;
    sprite->position = *position;
    sprite->active = 1;
    sprite->timer = 0;
    sprite->direction = direction;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    sprite->delay = (RandomValue(table) & 7) + delay;
    sprite->speed = (RandomValue(table) & 7) + 1;
    velocity = &sprite->velocity;
    velocity->x = Scaled(gUnk_086149C4[(direction + 64) & 255], speed);
    velocity->y = 0;
    velocity->z = Scaled(gUnk_086149C4[direction & 255], speed);
    p->velocity.y = LastRandom() & 31;
    p->mode = mode;
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

struct BurstSparkEntry {
    u8 unknown[0x3C];
    u8 kind;
    u8 unknown3D[2];
    u8 active;
};

struct BurstSparkManager {
    u8 unknown[0x38];
    struct BurstSparkEntry effects[128];
};

extern void *gUnk_08610764[];

s32 sub_08249240(void *, void *, void *);

s32 EffectPool_UpdateActive(struct BurstSparkManager *pool)
{
    void **callbacks = gUnk_08610764;
    struct BurstSparkEntry *effect = pool->effects;
    u8 *state = &effect->kind;
    s32 i = 127;
    do {
        if (state[3] == 1)
            sub_08249240(pool, effect, callbacks[state[0]]);
        effect++;
        state += 0x40;
    } while (--i >= 0);
    return 0;
}

u32 BurstSpark_DestroyPool(u8 *p)
{
    u8 *q = p + 0x38;
    u8 *r = p + 0x77;
    s32 n = 0x7f;
    u32 z;
    do {
        if (*r == 1 && q[4] != 0)
            Sprite_Remove(q);
        q += 0x40;
        r += 0x40;
        n--;
    } while (n >= 0);
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

s32 EffectPool_UpdateActive(struct BurstSparkManager *);

u32 BurstSpark_DestroyPool(u8 *);

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
