#include "scene_moving_particle.h"
#include "ram.h"
#include "scene_actor_chase_types.h"

/* SceneTerrainProbe_NoopHook: Empty function at the end of the tentative terrain probe subsystem */

void SceneTerrainProbe_NoopHook(void)
{
}

struct MovingParticleEntry *SceneMovingParticle_AllocateEntry(struct MovingParticlePool *pool)
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

void SceneMovingParticle_UpdateAnimation(struct MovingParticlePool *pool,
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

static inline s32 CellIndex(struct ContactPackedVector *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline s32 GroundHeight(s32 cell, struct ContactPackedVector *position)
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

static inline u8 Move(struct ContactPackedVector *position, struct ContactPosition *velocity,
                      s32 cell)
{
    s32 height;
    struct ContactPackedVector target;
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
    targetCell = CellIndex(&target);
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

void SceneMovingParticle_Update(struct MovingParticlePool *manager,
                                struct MovingParticleEntry *particle)
{
    if (particle->reset) {
        particle->sprite.flags &= ~1;
        particle->reset = 0;
    }
    if (!(particle->motionFlags & 1)) {
        particle->cell = CellIndex((struct ContactPackedVector *)&particle->sprite.position);
        particle->velocity.y--;
        if (Move((struct ContactPackedVector *)&particle->sprite.position, &particle->velocity,
                 particle->cell))
            particle->motionFlags |= 1;
    }
    SceneMovingParticle_UpdateAnimation(manager, particle);
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

struct MovingParticleEntry *SceneMovingParticle_Create(const struct ContactPosition *position,
                                                       const struct ContactPosition *velocity,
                                                       u32 lifetime, u32 direction)
{
    return SpriteBurst_Create(position, velocity, lifetime, direction, 1);
}

struct MovingParticleEntry *
SceneMovingParticle_CreateWithOptions(const struct ContactPosition *position,
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
        manager = SceneMovingParticle_EnsurePool();
        if (!manager)
            return 0;
    }
    entry = SceneMovingParticle_AllocateEntry(manager);
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

u32 SceneMovingParticle_UpdatePool(struct MovingParticlePool *pool)
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

u32 SceneMovingParticle_OnPoolDestroy(struct MovingParticlePool *pool)
{
    s32 i = 0;
    u32 *occupied = &pool->occupied;
    struct MovingParticleEntry *particle = pool->entries;
    for (; i < 26; particle++, i++) {
        if (MovingParticle_IsOccupied(occupied, i))
            Effect_Remove(particle);
    }
    {
        u32 z = 0;
        gSpriteBurstManager = z;
    }
    return 0;
}

s32 SceneMovingParticle_InitPool(struct MovingParticlePool *pool)
{
    pool->resource = AnimData_FindEntry(0x1c1e);
    pool->occupied = 0;
    gSpriteBurstManager = (u32)pool;
    return 0;
}

struct MovingParticlePool *SceneMovingParticle_EnsurePool(void)
{
    struct MovingParticlePool *pool;
    if (gSpriteBurstManager != 0)
        return (void *)gSpriteBurstManager;
    pool = Task_Create(10, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneMovingParticle_UpdatePool,
                          (void (*)(void *))SceneMovingParticle_OnPoolDestroy);
        if (SceneMovingParticle_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

void SceneMovingParticle_ResetPool(void)
{
    gSpriteBurstManager = 0;
}
