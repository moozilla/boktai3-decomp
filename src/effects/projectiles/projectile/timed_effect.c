#include "effects/projectiles/timed_effect.h"
#include "effects/projectiles/projectile_task.h"
#include "player/player.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_terrain.h"

/* TimedEffect_OnContact.c */

void Projectile_SetUpdateCallback(void *, void *);
void ProjectileTask_SpawnImpactChildren(void);
void Boss_FinalExplosions(void);
void ProjectileTask_StartTerminalAnimation(void *);

void TimedEffect_OnContact(struct ContactCollider *self,
                                 struct ContactCollider *other, void *owner)
{
    struct TimedEffect *entry = owner;
    struct TimedEffectConfig *config = &entry->config;
    struct ContactCollider *o;
    struct TimedEffect *e;

    if (config->type == 3) {
        entry->config.flags |= 0x80;
        Projectile_SetUpdateCallback(config, ProjectileTask_SpawnImpactChildren);
    } else if (config->type != 8) {
        if (config->type == 9) {
            Projectile_SetUpdateCallback(config, Boss_FinalExplosions);
        } else {
            e = entry;
            if ((self->contactFlags & 1) || (other->contactFlags & 1)) {
                o = other;
                if (!(o->contactFlags & 0x20)) {
                    e->config.flags |= 0x80;
                    ProjectileTask_StartTerminalAnimation(e);
                }
            }
        }
    }
}

/* sub_081DA0B4.c */

void Projectile_InitializeParams(void *);
void Sprite_InitUnlinked(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

static inline void FillWords(u32 *dst, int ctl, u32 *src)
{
    CpuSet(src, dst, ctl);
}

void TimedEffect_InitEntry(void *manager, struct Task4E01Projectile *entry, s32 index)
{
    struct Task4E01Params *config = &entry->params;
    void *collider;
    // concession-ok(const-local): CpuSet reads this fixed-source word by address.
    u32 zero;
    u32 *vel;

    Projectile_InitializeParams(config);
    entry->frame = 0;
    entry->runtime.restarted = 0;
    entry->runtime.index = index;
    vel = (u32 *)&entry->velocity;
    zero = 0;
    FillWords(vel, 0x05000002, &zero);
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

/* sub_081DA178.c */
void ProjectileTask_ClearTrailSprites(void *);
void Sprite_Remove(void *);
void TimedEffect_RemoveFromManager(struct TimedEffectManager *manager, struct TimedEffect *effect,
                                   s32 slot)
{
    struct TimedEffectConfig *config = &effect->config;
    struct ContactSwarmSprite *sprite;
    u32 flags;
    if (!(config->flags & 0x2010))
        manager->categoryCounts[config->category]--;
    manager->totalCounts[config->category]--;
    ProjectileTask_ClearTrailSprites(manager);
    sprite = &effect->sprite;
    flags = sprite->flags;
    sprite->flags = flags | 1;
    Sprite_Remove(sprite);
    manager->activeMask &= ~(1 << slot);
}

/* sub_081DA1DC.c */
// confidence=low: timed-effect projectile update that turns at terrain steps.

struct TimedEffectTerrainView {
    u32 frame;
    u8 pad4[0xC];
    struct ContactPosition velocity;
    struct TimedEffectConfig config;
    u8 pad[0x174 - 0x18 - sizeof(struct TimedEffectConfig)];
    u16 previous[4];
    s16 turn;
};
static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

void Projectile_SetUpdateCallback(void *, void *);
void ProjectileTask_SpawnImpactChildren(void);
void Sound_PlaySong(u32);
s32 ProjectileTask_SelectTerrainTurn(u16 *a, u16 *b, s32 original);

static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 speed)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * speed);
}

s32 TimedEffect_UpdateTerrainTurn(void *ctx, struct TimedEffectTerrainView *e, s32 index)
{
    struct ContactSwarmSprite *sp = (struct ContactSwarmSprite *)((u8 *)e + 0x8C);
    struct TimedEffectConfig *config = &e->config;
    u8 *attr;
    s32 type;
    attr = SceneActorTerrain_GetCell(CellIndex((u16 *)((u8 *)e + 0xA8)));
    if ((u32)((*attr & 15) << 8) > (u32)(s16)((u16 *)&sp->position)[1]) {
        type = config->type;
        if (type == 5 && e->frame != 0) {
            e->turn = ProjectileTask_SelectTerrainTurn((u16 *)&sp->position, e->previous, e->turn);
            {
                s32 n = e->turn;
                config->direction = (n << 5) - 0x60;
            }
            polar(&e->velocity, config->direction, config->speed);
            Sound_PlaySong(0x3D0);
            return 0;
        } else if (type == 3 && !(config->flags & 0x10)) {
            s16 a = (((u16)config->direction + 0x70) >> 5) & 7;
            u16 ub = a;
            s16 b = ub;
            s32 t = ProjectileTask_SelectTerrainTurn((u16 *)&sp->position, e->previous, e->turn);
            e->turn = t;
            if (!(t & 1)) {
                if ((s16)t == 0 && a == 6) {
                    e->turn = 1;
                } else if (e->turn == 6 && b == 0) {
                    e->turn = 5;
                } else {
                    e->turn = (e->turn - b) / 2 + (u16)e->turn;
                }
            }
            {
                s32 n = e->turn;
                config->direction = (n << 5) - 0x60;
            }
            config->flags |= 0x20;
            Projectile_SetUpdateCallback(config, ProjectileTask_SpawnImpactChildren);
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

/* sub_081DA3D8.c */

extern const s16 gUnk_086149C4[];
void TimedEffect_RemoveFromManager(struct TimedEffectManager *, struct TimedEffect *, s32);
s32 TimedEffect_UpdateTerrainTurn(void *, struct TimedEffectTerrainView *, s32);
void ProjectileTask_BeginImpactAnimation(struct TimedEffect *);

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Polar(struct ContactPosition *output, s32 direction, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
    output->x = x;
    output->y = 0;
    output->z = Scale(gUnk_086149C4[direction & 255] * speed);
}
s32 TimedEffect_UpdateManager(struct TimedEffectManager *manager)
{
    struct TimedEffect *entry = manager->entries;
    struct TimedEffect *cursor;
    struct TimedEffectTail *tail;
    s32 i;
    i = 0;
    if (i < manager->capacity) {
        cursor = entry;
        tail = &entry->tail;
        do {
            if (manager->activeMask & (1 << i)) {
                struct ContactSwarmSprite *sprite = &entry->sprite;
                struct TimedEffectConfig *config = &entry->config;
                struct ContactCollider *collider;
                s32 status = sub_08249240(entry, (void *)i, cursor->config.callback);
                collider = &entry->colliders[0];
                if (status == 2) {
                    TimedEffect_RemoveFromManager(manager, entry, i);
                    goto next;
                }
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

                        tail->previous = *position;
                        sprite->position.x += cursor->velocity.x;
                        position->y += cursor->velocity.y;
                        position->z += cursor->velocity.z;
                        type = config->type;
                        rollbackPosition = position;
                        if (type == 9 || type == 6)
                            sprite->position.y -= tail->elapsed;
                        savedVelocity = &velocity;
                        velocity = cursor->velocity;
                        status = TimedEffect_UpdateTerrainTurn(
                            manager, (struct TimedEffectTerrainView *)entry, i);
                        if (status <= 0) {
                            if (status == -1) {
                                config->flags |= 0x20;
                                ProjectileTask_BeginImpactAnimation(entry);
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
                        struct Player *player = gPlayers[0];
                        u32 flag = 0x80000;
                        player->statusFlags |= flag;
                    }
                }
                cursor->frame++;
            }
        next:
            i++;
            cursor++;

            tail = (void *)((u8 *)tail + sizeof(*entry));
            entry++;
        } while (i < manager->capacity);
    }
    return 0;
}

/* sub_081DA61C.c */
void Effect_Remove(void *);
void Heap_Free(void *);
void TimedEffect_RemoveFromManager(struct TimedEffectManager *, struct TimedEffect *, s32);
void ResourceRegistry_UnregisterResource(u16);
s32 TimedEffect_DestroyManager(struct TimedEffectManager *manager)
{
    struct TimedEffectManagerSprite *effect = manager->effects;
    s32 i = 3;
    struct TimedEffect *entry;
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

/* sub_081DA684.c */

s32 ResourceRegistry_RegisterResource(u32, void *, u32);
void SpriteGroup_InitFourSprites(void *, u32);

s32 TimedEffect_InitManager(struct TimedEffectManager *manager, s32 count)
{
    u8 *entry;
    s32 i;
    u8 *heap;

    manager->capacity = count;
    heap = Heap_Alloc(count * 0x1AC);
    manager->entries = heap;
    if (heap == NULL)
        return -1;
    Memory_Clear(heap, manager->capacity * 0x1AC);
    ResourceRegistry_RegisterResource(0x4E01, manager, 0);
    manager->activeMask = 0;
    entry = manager->entries;
    for (i = 0; i < manager->capacity;) {
        TimedEffect_InitEntry(manager, (struct Task4E01Projectile *)entry, i);
        i++;
        entry += 0x1AC;
    }
    for (i = 0; i < 4; i++) {
        manager->categoryCounts[i] = 0;
        manager->totalCounts[i] = 0;
    }
    SpriteGroup_InitFourSprites(manager, 3);
    return 0;
}

/* sub_081DA720.c */

void *ResourceRegistry_GetResource(u32);
/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by TimedEffect_InitManager; NULL when that fails (tentative). */
u8 *ProjectileTask_GetOrCreate(void)
{
    u8 *p = ResourceRegistry_GetResource(0x4E01);
    if (p)
        return p;
    p = Task_Create(8, 0x10C);
    if (p) {
        Task_SetCallbacks(p, TimedEffect_UpdateManager, TimedEffect_DestroyManager);
        if (TimedEffect_InitManager(p, 12) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

/* sub_081DA770.c */

void *ResourceRegistry_GetResource(u32);
/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by TimedEffect_InitManager; NULL when that fails (tentative). */
u8 *TimedEffect_GetOrCreateManager(s32 capacity)
{
    u8 *p = ResourceRegistry_GetResource(0x4E01);
    if (p)
        return p;
    if (capacity > 32)
        capacity = 32;
    if (capacity < 12)
        capacity = 12;
    p = Task_Create(8, 0x10C);
    if (p) {
        Task_SetCallbacks(p, TimedEffect_UpdateManager, TimedEffect_DestroyManager);
        if (TimedEffect_InitManager(p, capacity) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

/* sub_081DA7D0.c */
struct TimedEffect *TimedEffect_FindFreeEntry(struct TimedEffectManager *manager, s32 *slot)
{
    struct TimedEffect *entry = manager->entries;
    s32 i;
    for (i = 0; i < manager->capacity; i++, entry++) {
        if (!(manager->activeMask & (1 << i))) {
            *slot = i;
            return entry;
        }
    }
    return 0;
}
