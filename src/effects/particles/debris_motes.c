#include "effects/particles/scene_moving_particle.h"
#include "engine/ram.h"
#include "scene/motion/scene_actor_chase_types.h"
#include "scene/terrain/terrain_sampling.h"

/* DebrisMote_NoopHook: Empty function at the end of the tentative terrain probe subsystem */

void DebrisMote_NoopHook(void)
{
}

struct MovingParticleEntry *DebrisMote_AllocateEntry(struct MovingParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 26; i++) {
        if (!MovingParticle_IsOccupied(&pool->occupied, i)) {
            struct MovingParticleEntry *entry = &pool->entries[i];
            pool->occupied |= 1 << i;
            entry->index = i;
            return entry;
        }
    }
    return 0;
}

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void DebrisMote_UpdateAnimation(struct MovingParticlePool *pool,
                                         struct MovingParticleEntry *particle)
{
    if (particle->animationTimer != 0) {
        particle->animationTimer--;
    } else {
        particle->animationTimer = 3;
        particle->animationFrame = (particle->animationFrame + 1) & 1;
        SpriteDrawable_SelectTileFrame(particle, pool->resource,
                                       particle->frame + particle->animationFrame);
    }
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Div(s32, s32);

static inline s32 GroundHeight(s32 cell, struct ContactPosition *position)
{
    u8 *definition = TerrainModifiers_FindCellOverride(cell, 1);
    u32 kind;
    s32 height;
    if (definition)
        definition = (void *)&((struct SceneActorChaseTerrainOverride *)definition)->properties;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = (u32) & ((struct SceneActorChaseTerrainGrid *)0)->cells[cell];
        definition = map->cells + offset;
    }
    kind = ((struct SceneActorChaseTerrainCell *)definition)->attributes >> 4;
    height = (((struct SceneActorChaseTerrainCell *)definition)->attributes & 15) << 8;
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

static inline u8 Move(struct ContactPosition *position, struct ContactPosition *velocity, s32 cell)
{
    s32 height;
    struct ContactPosition target;
    s32 targetCell;
    Math_Atan2(velocity->x, velocity->z);
    height = GroundHeight(cell, position);
    if (velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z == 0)
        return 1;
    if (position->y <= height) {
        velocity->x = Div(velocity->x * 6, 10);
        velocity->y = Div(-(velocity->y * 4), 10);
        velocity->z = Div(velocity->z * 6, 10);
        position->y = height;
    }
    target.x = position->x + velocity->x;
    target.y = position->y + velocity->y;
    target.z = position->z + velocity->z;
    targetCell = Terrain_GetCellIndex(&target);
    if (cell != targetCell) {
        if (height < GroundHeight(targetCell, &target)) {
            velocity->x = -Div(velocity->x * 5, 10);
            velocity->z = -Div(velocity->z * 5, 10);
        }
    }
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
    return 0;
}

void DebrisMote_Update(struct MovingParticlePool *manager,
                                struct MovingParticleEntry *particle)
{
    if (particle->reset) {
        particle->sprite.flags &= ~1;
        particle->reset = 0;
    }
    if (!(particle->motionFlags & 1)) {
        particle->cell = Terrain_GetCellIndex(&particle->sprite.position);
        particle->velocity.y--;
        if (Move(&particle->sprite.position, &particle->velocity, particle->cell))
            particle->motionFlags |= 1;
    }
    DebrisMote_UpdateAnimation(manager, particle);
    if (particle->lifetime)
        particle->lifetime--;
    else {
        particle->state = 2;
        particle->reset = 1;
    }
}

void EffectSlot_Release(void *unused, struct MovingParticleEntry *slot)
{
    if (slot->reset)
        slot->reset = 0;
    {
        struct MovingParticlePool *pool = (void *)gSpriteBurstManager;
        s32 index = slot->index;
        pool->occupied &= ~(1 << index);
    }
    slot->state = 0;
    Effect_Remove(slot);
    slot->index = -1;
    slot->motionFlags = 0;
    slot->lifetime = 0;
}

struct MovingParticleEntry *DebrisMote_Create(const struct ContactPosition *position,
                                                       const struct ContactPosition *velocity,
                                                       u32 lifetime, u32 direction)
{
    return SpriteBurst_Create(position, velocity, lifetime, direction, 1);
}

struct MovingParticleEntry *
DebrisMote_CreateWithOptions(const struct ContactPosition *position,
                                      const struct ContactPosition *velocity, u32 lifetime,
                                      u32 direction, u32 priority)
{
    return SpriteBurst_Create(position, velocity, lifetime, direction, priority);
}

void ParticleSprite_Initialize(struct MovingParticleEntry *, void *, u32);

void Effect_SetOrigin(struct MovingParticleEntry *, s32, s32);

void Effect_SetPalette(struct MovingParticleEntry *, u32);

struct MovingParticleEntry *SpriteBurst_Create(const struct ContactPosition *position,
                                               const struct ContactPosition *velocity, u32 lifetime,
                                               u32 direction, u32 priority)
{
    struct MovingParticlePool *manager = (void *)gSpriteBurstManager;
    struct MovingParticleEntry *entry;
    u32 frame;
    if (!manager) {
        manager = DebrisMote_EnsurePool();
        if (!manager)
            return 0;
    }
    entry = DebrisMote_AllocateEntry(manager);
    if (!entry)
        return 0;
    entry->state = 1;
    entry->motionFlags = 0;
    entry->reset = 1;
    entry->velocity = *velocity;
    entry->lifetime = lifetime;
    entry->initialDelay = 30;
    entry->animationTimer = 0;
    switch (direction) {
        case 1:
            frame = 1;
            break;
        case 2:
            frame = 2;
            break;
        case 3:
            frame = 3;
            break;
        case 4:
            frame = 4;
            break;
        case 5:
            frame = 5;
            break;
        case 6:
            frame = 6;
            break;
        case 7:
            frame = 7;
            break;
        case 0:
        default:
            frame = 0;
            break;
    }
    entry->animationFrame = 0;
    entry->frame = frame * 2;
    entry->sprite.position = *position;
    ParticleSprite_Initialize(entry, manager->resource, 0);
    Effect_SetOrigin(entry, -4, -4);
    SpriteDrawable_SelectTileFrame(entry, manager->resource, entry->frame);
    Effect_SetPalette(entry, 1);
    entry->sprite.flags |= 1;
    entry->sprite.priority = priority;
    return entry;
}

extern MovingParticleUpdate gUnk_08610740[];

u32 DebrisMote_UpdatePool(struct MovingParticlePool *pool)
{
    struct MovingParticleEntry *start = pool->entries;
    struct MovingParticleEntry *entry;
    s32 i;
    u32 *occupied;
    u8 *state;
    MovingParticleUpdate *callbacks;
    if (!pool->occupied)
        return 0;
    i = 0;
    occupied = MovingParticle_SlotMask(pool);
    callbacks = gUnk_08610740;
    state = &pool->entries[0].state;
    entry = start;
    for (; i < 26; state += sizeof(*entry), entry++, i++) {
        if (MovingParticle_IsOccupied(occupied, i))
            callbacks[*state](pool, entry);
    }
    return 0;
}

u32 DebrisMote_OnPoolDestroy(struct MovingParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 26; i++) {
        if (MovingParticle_IsOccupied(&pool->occupied, i))
            Effect_Remove(&pool->entries[i].sprite);
    }
    gSpriteBurstManager = 0;
    return 0;
}

s32 DebrisMote_InitPool(struct MovingParticlePool *pool)
{
    pool->resource = AnimData_FindEntry(0x1c1e);
    pool->occupied = 0;
    gSpriteBurstManager = (u32)pool;
    return 0;
}

struct MovingParticlePool *DebrisMote_EnsurePool(void)
{
    struct MovingParticlePool *pool;
    if (gSpriteBurstManager != 0)
        return (void *)gSpriteBurstManager;
    pool = Task_Create(10, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))DebrisMote_UpdatePool,
                          (void (*)(void *))DebrisMote_OnPoolDestroy);
        if (DebrisMote_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

void DebrisMote_ResetPool(void)
{
    gSpriteBurstManager = 0;
}
