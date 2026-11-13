#include "timed_effect_manager.h"
#include "scene_actor_terrain.h"
#include "player.h"

void Task4E01_SpawnImpactChildren(void);

void Boss_FinalExplosions(void);

void Task4E01_StartTerminalAnimation(void *);

void TimedEffect_OnContact(struct ContactCollider *self, struct ContactCollider *other,
                           struct Task4E01Projectile *entry)
{
    struct Task4E01Params *config = &entry->params;
    struct ContactCollider *o;
    struct Task4E01Projectile *e;

    if (config->type == 3) {
        (*(&entry))->params.flags |= 0x80;
        Projectile_SetUpdateCallback(config, Task4E01_SpawnImpactChildren);
    } else if (config->type != 8) {
        if (config->type == 9) {
            Projectile_SetUpdateCallback(config, Boss_FinalExplosions);
        } else {
            e = entry;
            if ((self->contactFlags & 1) || (other->contactFlags & 1)) {
                o = other;
                if (!(o->contactFlags & 0x20)) {
                    e->params.flags |= 0x80;
                    Task4E01_StartTerminalAnimation(e);
                }
            }
        }
    }
}

void Projectile_InitializeParams(void *);

void Sprite_InitUnlinked(void *, void *, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

static inline void FillWords(u32 *dst, int ctl, u32 *src)
{
    CpuSet(src, dst, ctl);
}

void TimedEffect_InitEntry(struct TimedEffectManager *manager, struct Task4E01Projectile *entry,
                           s32 index)
{
    struct Task4E01Params *config = &entry->params;
    void *collider;
    u32 zero;
    struct ContactPosition *vel;

    Projectile_InitializeParams(config);
    entry->frame = 0;
    entry->runtime.restarted = 0;
    entry->runtime.index = index;
    vel = &entry->velocity;
    zero = 0;
    FillWords((u32 *)vel, 0x05000002, &zero);
    AnimData_Load(&entry->graphics, config->sprite[0]);
    Sprite_InitUnlinked(&entry->sprite, &entry->graphics, 1);
    collider = entry->colliders;
    if (entry->params.flags & 0x8000)
        Collider_Init(collider, 0, 0x2101, 0, 16, &entry->params.bounds, &entry->params.offset);
    else
        Collider_Init(collider, 0, 0x2101, 0, 1, &entry->params.bounds, &entry->params.offset);
    Collider_SetCallback(collider, TimedEffect_OnContact, entry);
    Collider_SetAttackProperties(collider, config->damage[2], config->damage[3], config->damage[0],
                                 config->damage[1], config->damage[4]);
}

void Task4E01_ClearTrailSprites(void *);

void TimedEffect_RemoveFromManager(struct TimedEffectManager *manager,
                                   struct Task4E01Projectile *effect, u32 slot)
{
    struct Task4E01Params *config = &effect->params;
    struct ContactSwarmSprite *sprite;
    u32 one;
    u32 flags;
    if (!(config->flags & 0x2010))
        manager->counts[config->variant]--;
    manager->totalCounts[config->variant]--;
    Task4E01_ClearTrailSprites(manager);
    sprite = &effect->sprite;
    flags = sprite->flags;
    one = 1;
    sprite->flags = flags | one;
    Sprite_Remove(sprite);
    manager->activeMask &= ~(one << slot);
}

// confidence=low: timed-effect projectile update that turns at terrain steps.

static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

void Sound_PlaySong(u32);

s32 Task4E01_SelectTerrainTurn(u16 *a, u16 *b, s32 original);

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void Polar(struct ContactPosition *output, s32 direction, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
    s32 zero = 0;
    output->x = x;
    output->y = zero;
    output->z = Scale(gUnk_086149C4[direction & 255] * speed);
}

s32 TimedEffect_UpdateTerrainTurn(struct TimedEffectManager *ctx,
                                  struct Task4E01Projectile *projectile, s32 index)
{
    struct ContactSwarmSprite *sprite = &projectile->sprite;
    struct Task4E01Params *config = &projectile->params;
    u8 *attributes;
    s32 type;
    attributes = SceneActorTerrain_GetCell(CellIndex((u16 *)&projectile->sprite.position));
    if ((u32)((*attributes & 15) << 8) > (u32)(s16)sprite->position.y) {
        type = config->type;
        if (type == 5 && projectile->frame != 0) {
            projectile->runtime.turn = Task4E01_SelectTerrainTurn(
                (u16 *)&sprite->position, (u16 *)&projectile->runtime.previousPosition,
                projectile->runtime.turn);
            {
                s32 n = projectile->runtime.turn;
                config->direction = (n << 5) - 0x60;
            }
            Polar(&projectile->velocity, config->direction, config->speed);
            Sound_PlaySong(0x3D0);
            return 0;
        } else if (type == 3 && !(config->flags & 0x10)) {
            s16 a = (((u16)config->direction + 0x70) >> 5) & 7;
            u16 ub = a;
            s16 b = ub;
            s32 t = Task4E01_SelectTerrainTurn((u16 *)&sprite->position,
                                               (u16 *)&projectile->runtime.previousPosition,
                                               projectile->runtime.turn);
            projectile->runtime.turn = t;
            if (!(t & 1)) {
                if ((s16)t == 0 && a == 6) {
                    projectile->runtime.turn = 1;
                } else if (projectile->runtime.turn == 6 && b == 0) {
                    projectile->runtime.turn = 5;
                } else {
                    projectile->runtime.turn =
                        (projectile->runtime.turn - b) / 2 + (u16)projectile->runtime.turn;
                }
            }
            {
                s32 n = projectile->runtime.turn;
                config->direction = (n << 5) - 0x60;
            }
            config->flags |= 0x20;
            Projectile_SetUpdateCallback(config, Task4E01_SpawnImpactChildren);
            Sound_PlaySong(0x161);
            return 0;
        } else if (type == 9) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return 1;
    }
}

extern struct Player *gUnknown_02000580;

extern const s16 gUnk_086149C4[];

void Task4E01_BeginImpactAnimation(struct Task4E01Projectile *);

s32 TimedEffect_UpdateManager(struct TimedEffectManager *manager)
{
    struct Task4E01Projectile *entry = manager->entries;
    struct Task4E01Projectile *cursor;
    struct Task4E01Runtime *tail;
    s32 i;
    i = 0;
    if (i < manager->capacity) {
        cursor = entry;
        tail = &entry->runtime;
        do {
            if (manager->activeMask & (1 << i)) {
                struct ContactSwarmSprite *sprite = &entry->sprite;
                struct Task4E01Params *config = &entry->params;
                struct ContactCollider *collider;
                s32 status = sub_08249240(entry, (void *)i, cursor->params.callback);
                collider = &entry->colliders[0];
                if (status == 2) {
                    TimedEffect_RemoveFromManager(manager, entry, i);
                } else {
                    if (cursor->flags & 1) {
                        Polar(&entry->velocity, config->direction, config->speed);
                        {
                            s32 mask = ~1;
                            cursor->flags &= mask;
                        }
                    }
                    if (config->flags & 0x1000) {
                        struct ContactPosition *position = &sprite->position;
                        sprite->position.x += cursor->velocity.x;
                        position->y += cursor->velocity.y;
                        position->z += cursor->velocity.z;
                    } else {
                        s32 zero = 0;
                        if (status != 1) {
                            struct ContactPosition *position = &sprite->position;
                            struct ContactPosition *rollbackPosition;
                            struct ContactPosition velocity;
                            struct ContactPosition *savedVelocity;
                            s32 type;

                            tail->previousPosition = *position;
                            sprite->position.x += cursor->velocity.x;
                            position->y += cursor->velocity.y;
                            position->z += cursor->velocity.z;
                            type = config->type;
                            rollbackPosition = position;
                            if (type == 9 || type == 6)
                                sprite->position.y -= tail->elapsed;
                            savedVelocity = &velocity;
                            velocity = cursor->velocity;
                            status = TimedEffect_UpdateTerrainTurn(manager, entry, i);
                            if (status <= 0) {
                                if (status == -1) {
                                    config->flags |= 0x20;
                                    Task4E01_BeginImpactAnimation(entry);
                                } else if (config->type != 9) {
                                    sprite->position.x -= savedVelocity->x;
                                    rollbackPosition->y -= savedVelocity->y;
                                    rollbackPosition->z -= savedVelocity->z;
                                }
                            }
                        }
                        if (tail->cooldown > 0) {
                            {
                                s32 mask = ~2;
                                cursor->flags &= mask;
                            }
                            tail->cooldown--;
                        }
                        entry->colliders[1].flags |= 4;
                        // The ROM keeps this always-true test (movs r2, #0; cmp r2, #0).
                        if (config->type != 9 && !zero) {
                            u32 mask = 2;
                            if (cursor->flags & mask) {
                                Collider_SetPosition(collider, &sprite->position, zero);
                                Collider_Refresh(collider);
                            }
                        }
                        {
                            struct Player *player = gUnknown_02000580;
                            u32 flag = 0x80000;
                            player->statusFlags |= flag;
                        }
                    }
                    cursor->frame++;
                }
            }
            i++;
            cursor++;

            tail = (void *)((u8 *)tail + sizeof(*entry));
            entry++;
        } while (i < manager->capacity);
    }
    return 0;
}

void Effect_Remove(void *);

void ResourceRegistry_UnregisterResource(u16);

s32 TimedEffect_DestroyManager(struct TimedEffectManager *manager)
{
    struct TimedEffectTrail *effect = manager->effects;
    s32 i = 3;
    struct Task4E01Projectile *entry;
    do {
        Effect_Remove(effect);
        effect++;
    } while (--i >= 0);
    entry = manager->entries;
    for (i = 0; i < manager->capacity; i++, entry++) {
        if (manager->activeMask & (1 << i))
            TimedEffect_RemoveFromManager(manager, entry, i);
    }
    Heap_Free(manager->entries);
    ResourceRegistry_UnregisterResource(0x4E01);
    return 0;
}

s32 ResourceRegistry_RegisterResource(u32, void *, u32);

void SpriteGroup_InitFourSprites(void *, u32);

s32 TimedEffect_InitManager(struct TimedEffectManager *manager, s32 count)
{
    u8 *entry;
    s32 i;
    u8 *heap;

    manager->capacity = count;
    heap = Heap_Alloc(count * sizeof(struct Task4E01Projectile));
    manager->entries = heap;
    if (heap == NULL)
        return -1;
    Memory_Clear(heap, manager->capacity * sizeof(struct Task4E01Projectile));
    ResourceRegistry_RegisterResource(0x4E01, manager, 0);
    manager->activeMask = 0;
    entry = manager->entries;
    for (i = 0; i < manager->capacity;) {
        TimedEffect_InitEntry(manager, entry, i);
        i++;
        entry += sizeof(struct Task4E01Projectile);
    }
    for (i = 0; i < 4; i++) {
        manager->counts[i] = 0;
        manager->totalCounts[i] = 0;
    }
    SpriteGroup_InitFourSprites(manager, 3);
    return 0;
}

void *ResourceRegistry_GetResource(u32);

/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by TimedEffect_InitManager; NULL when that fails (tentative). */

struct TimedEffectManager *Task4E01_GetOrCreate(void)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4E01);
    if (manager)
        return manager;
    manager = Task_Create(8, 0x10C);
    if (manager) {
        Task_SetCallbacks(manager, TimedEffect_UpdateManager, TimedEffect_DestroyManager);
        if (TimedEffect_InitManager(manager, 12) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by TimedEffect_InitManager; NULL when that fails (tentative). */

struct TimedEffectManager *TimedEffect_GetOrCreateManager(s32 capacity)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4E01);
    if (manager)
        return manager;
    if (capacity > 32)
        capacity = 32;
    if (capacity < 12)
        capacity = 12;
    manager = Task_Create(8, 0x10C);
    if (manager) {
        Task_SetCallbacks(manager, TimedEffect_UpdateManager, TimedEffect_DestroyManager);
        if (TimedEffect_InitManager(manager, capacity) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct Task4E01Projectile *TimedEffect_FindFreeEntry(struct TimedEffectManager *manager, s32 *slot)
{
    struct Task4E01Projectile *entry = manager->entries;
    s32 i;
    for (i = 0; i < manager->capacity; i++, entry++) {
        if (!(manager->activeMask & (1 << i))) {
            *slot = i;
            return entry;
        }
    }
    return 0;
}
