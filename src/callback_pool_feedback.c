#include "global.h"
#include "callback_pool.h"
#include "engine_globals.h"
#include "ram.h"
#include "scene_object.h"
#include "engine.h"

s32 Mod(s32, s32);

static inline u16 CallbackPool_NextRandom(void)
{
    u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}

void CallbackPool_UpdateShake(struct CallbackPoolEntry *entry)
{
    if (entry->shakeTimer == 0) {
        entry->sprite.position = entry->savedPosition;
    } else {
        entry->sprite.position.x = entry->savedPosition.x + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->sprite.position.z = entry->savedPosition.z + Mod(CallbackPool_NextRandom(), 20) - 10;
        entry->shakeTimer--;
    }
}

void Collision_CalculateDamageResponse(void *, void *);

void SceneHud_SetTimedState(void *, s32, s32, u32, s32, s32);

void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void Sound_PlaySong(u32);

static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}

void CallbackPool_ApplyDamage(struct ContactCollider *source, struct ContactCollider *target,
                              struct CallbackPoolEntry *entry)
{

    s32 damage;
    u32 stack[3];
    u32 *vector;
    if (entry->collider.responseValue != 0)
        return;
    Collision_CalculateDamageResponse(source, target);
    target->responseValue = source->responseValue;
    damage = entry->health - entry->collider.cost;
    if (damage < 0)
        damage += entry->collider.cost;
    else
        damage = entry->collider.cost;
    entry->health -= entry->collider.cost;
    if (entry->health < 0)
        entry->health = 0;
    SceneHud_SetTimedState(entry->healthDisplay, entry->health, entry->maxHealth,
                           entry->displayMode, 0, -1);
    vector = stack + 1;
    ClearVector(vector, stack);
    DamageNumberDisplay_Show(target, damage, 0, &entry->sprite.position, vector, 1);
    entry->hitTimer = 3;
    entry->shakeTimer = 10;
    CallbackPool_SpawnHitParticles(entry);
    CallbackPool_RefreshHealthDisplay(entry);
    if (entry->collider.cost)
        Sound_PlaySong(0x13e);
    else
        Sound_PlaySong(0x211);
    if (entry->health <= 0) {
        entry->callbackIndex = 4;
        entry->defeated = 1;
    }
}

void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

static inline s32 NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}

// confidence=low: spawns a small burst of hit particles at the entry's position.

void CallbackPool_SpawnHitParticles(struct CallbackPoolEntry *entry)
{
    u32 stack[3];
    struct SceneExtent spread;
    struct SceneExtent *velocity = (struct SceneExtent *)(stack + 1);
    ClearVector(velocity, stack);
    ((u16 *)velocity)[1] = 16;
    spread.x = (NextRandom() & 3U) + 15U;
    spread.y = (NextRandom() & 7U) + 15U;
    spread.z = (NextRandom() & 3U) + 15U;
    sub_08014634((NextRandom() & 3) + 2, 17, (struct SceneExtent *)&entry->sprite.position,
                 velocity, &spread, 12, 15);
}

void CallbackPool_ApplyDamage(struct ContactCollider *, struct ContactCollider *,
                              struct CallbackPoolEntry *);

/* CallbackPool_InitializeCollider: retain the address symbol used by the earlier constructor. */

void sub_08105194(struct CallbackPoolEntry *entry, u32 a, u32 b, u32 c)
{
    struct SceneExtent v, w;
    struct ContactCollider *collider = &entry->collider;
    w.x = 0;
    w.y = 0x64;
    w.z = 0x40;
    v.x = 0x40;
    v.y = 0x3C;
    v.z = 0x40;
    collider->collisionId = (u32)entry->healthDisplay;
    Collider_Init(collider, 0, 0x5001, 0, 0x10, &v, &w);
    Collider_SetBodyProperties(collider, a, b, c);
    Collider_SetPosition(collider, &entry->sprite.position, 0);
    Collider_SetCallback(collider, CallbackPool_ApplyDamage, entry);
    Collider_Refresh(collider);
}

extern const s16 gUnk_086149C4[];

void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline u16 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)gRandomTable + index);
}

static inline void Polar(s32 angle, struct ContactPosition *v, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

// confidence=low: removal caller emits five directional debris particles.

void ActorGroup_EmitRemovalDebris(struct CallbackPoolEntry *entry)
{
    struct ContactPosition v;
    {
        struct ContactPosition *velocity = &v;
        s32 angle = Rand() & 255;
        s32 direction = angle;
        struct ContactPosition *selected = velocity;
        Polar(direction, selected, 10);
    }
    v.y = 64;
    sub_08127590(&entry->sprite.position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&entry->sprite.position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&entry->sprite.position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 0, 1, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&entry->sprite.position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 1, 0, 0, 0, 0, 0);
    Polar(Rand() & 255, &v, 10);
    v.y = 34;
    sub_08127590(&entry->sprite.position, &v, 0x9385, 0x74C9, 1, 30, 2, 0, 490, 2, 0, 0, 0, 0, 0);
    CallbackPool_SpawnHitParticles(entry);
}

static inline u32 ProximityActor_NextSpawnSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u16 *)((gRandomIndex << 1) + (u32)table);
}

void ProximityActor_SetState(struct CallbackPoolEntry *entry, s32 count)
{
    s32 values[4];
    struct CallbackPoolScriptArgs args;
    s32 i;
    if (count > 0 && entry->spawnScript) {
        args.count = 4;
        args.values = values;
        for (i = 0; i < count; i++) {
            union RandomTableView table = {gRandomTable};
            {
                u32 random = ProximityActor_NextSpawnSample(table.samples);
                s32 position = entry->sprite.position.x - 64;
                random &= 127;
                values[0] = position + random;
            }
            {
                u32 random = ProximityActor_NextSpawnSample(table.samples);
                s32 position = entry->sprite.position.y - 64;
                random &= 127;
                values[1] = position + random;
            }
            {
                u32 random = ProximityActor_NextSpawnSample(table.samples);
                s32 position = entry->sprite.position.z - 64;
                random &= 127;
                values[2] = position + random;
            }
            values[3] = entry->spawnIndex;
            Script_Call(entry->spawnScript, &args);
            entry->spawnIndex++;
        }
    }
}

void CallbackPool_RefreshHealthDisplay(struct CallbackPoolEntry *entry)
{
    u8 *level;
    if (entry->spawnTarget > *(level = &entry->spawnIndex)) {
        s32 diff = entry->maxHealth - entry->health;
        if (!(u8)Pool184_TestStatusFlags(entry, 1))
            ProximityActor_SetState(entry, 2);
        if (diff >= entry->damageThreshold * (*level - 1))
            ProximityActor_SetState(entry, 1);
    }
}

void ProximityActor_CheckPlayerRange(struct CallbackPoolEntry *actor)
{
    struct ContactPosition delta;
    s32 distance;
    if (!(u8)Pool184_TestStatusFlags(actor, 1)) {
        CallbackPool_Subtract(&delta, &actor->sprite.position, &gPlayers[0]->position);
        distance = delta.x * delta.x + delta.z * delta.z;
        actor->distanceSquared = distance;
        if (actor->distanceSquared < actor->triggerDistanceSquared) {
            ProximityActor_SetState(actor, 2);
            Pool184_AddStatusFlags(actor, 1);
            Sound_PlaySong(0x3C7);
        }
    }
}

s32 ProximityActor_IsPlayerInsideOriginRange(struct CallbackPoolEntry *actor)
{
    struct ContactPosition delta;
    CallbackPool_Subtract(&delta, &actor->savedPosition, &gPlayers[0]->position);
    actor->previousDistanceSquared = actor->distanceSquared;
    actor->distanceSquared = delta.x * delta.x + delta.z * delta.z;
    if (actor->distanceSquared < actor->triggerDistanceSquared)
        return 1;
    return 0;
}
