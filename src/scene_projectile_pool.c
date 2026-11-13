#include "scene_projectile_pool.h"
#include "player.h"
#include "display_sequence_inline.h"

u32 SceneProjectile_ConsumeHitEvent(void)
{
    struct SceneProjectilePool *g = gSceneProjectilePool;
    u32 r;
    if (g->hitEvent == 0)
        r = 0;
    else {
        g->hitEvent = 0;
        r = 1;
    }
    return r;
}

void SceneProjectile_UpdateSpriteRotation(struct SceneProjectile *projectile)
{

    u32 rotation;
    projectile->sprite.flags = (projectile->sprite.flags & ~2) | 2;
    rotation = projectile->launch.rotation;
    projectile->launch.rotation = rotation + 0x10;
    projectile->sprite.rotation = -(rotation + 0x70);
}

s32 Mod(s32, s32);

void Projectile_AdjustHeading(void *);

s32 SceneProjectile_UpdatePulse(struct SceneProjectile *projectile)
{
    if (projectile->age > projectile->params.lifetime)
        return 1;
    if (Mod(projectile->age, 2) == 0)
        Projectile_AdjustHeading(projectile);
    if (projectile->age == 1) {
        s32 v = projectile->params.turnSpeed >> 2;
        projectile->params.turnSpeed = v;
        if (v <= 0)
            projectile->params.turnSpeed = 1;
    } else if (projectile->age == 30) {
        projectile->params.turnSpeed <<= 2;
    } else if (projectile->age == 90 || projectile->age == 120) {
        s32 v = projectile->params.turnSpeed >> 1;
        projectile->params.turnSpeed = v;
        if (v <= 0)
            projectile->params.turnSpeed = 1;
    }
    if (projectile->age < 120) {
        if (Mod(projectile->age, 10) == 0)
            projectile->params.speed--;
    }
    if (projectile->age >= 120) {
        if (Mod(projectile->age, 10) == 0)
            projectile->params.speed += 2;
    }
    {
        u32 mask = 2;
        projectile->flags = mask | projectile->flags;
    }
    return 0;
}

extern u32 gRandomIndex;

void SlotMotion_Initialize(struct SceneParticleLaunchParams *);

void SceneParticles_SpawnAtPosition(struct SceneParticleLaunchParams *,
                                    struct OrbitEffectPosition *);

void Ratatosk_ConfigureParticle(struct SceneParticleLaunchParams *, u16, u32, u8, u32, u32);

s32 SceneProjectile_EmitPeriodicParticle(struct SceneProjectile *projectile)
{
    struct SceneParticleLaunchParams particle;
    u32 scale;
    u32 offset;
    if (projectile->age > projectile->params.lifetime)
        return 1;
    {
        u32 mask = 2;
        projectile->flags = mask | projectile->flags;
    }
    if (Mod(projectile->age, 8) == 0) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        scale = *(const u16 *)(offset + (u32)gRandomTable);
        scale &= 1;
        scale += 1;
        SlotMotion_Initialize(&particle);
        Ratatosk_ConfigureParticle(&particle, 40, 0, 0, 1, scale << 9);
        SceneParticles_SpawnAtPosition(&particle, &projectile->sprite.position);
    }
    return 0;
}

u32 SceneProjectile_UpdateHomingRotation(struct SceneProjectile *projectile)
{
    u32 r;
    if (projectile->age <= projectile->params.lifetime) {
        Projectile_AdjustHeading(projectile);
        {
            u32 m = 2;
            projectile->flags = m | projectile->flags;
        }
        SceneProjectile_UpdateSpriteRotation(projectile);
        r = 0;
    } else
        r = 1;
    return r;
}

s32 Player_StartForcedStun(u8 *, s32);

void Sound_PlaySong(u32);

void SceneProjectile_RespondToContact(void *unused, struct ContactCollider *contact,
                                      struct SceneProjectile *projectile)
{
    struct SceneProjectileParams *state = &projectile->params;
    if (projectile->params.mode == 1 && contact->contactFlags == 1)
        ((struct SceneProjectilePool *)gSceneProjectilePool)->hitEvent = 1;
    if (projectile->params.mode == 0) {
        if (gPlayers[0]->tetherMode != 26 && gPlayers[0]->positionResetTimer == 0) {
            if (Player_StartForcedStun((u8 *)gPlayers[0], 90) == 1)
                Sound_PlaySong(1400);
        }
    }
    if (projectile->params.mode != 2) {
        projectile->age = state->lifetime;
    } else if (contact->contactFlags == 32) {
        projectile->launch.contactCount++;
        if (projectile->launch.contactCount <= 1 && state->contactTimer <= -10) {
            state->contactTimer = 120;
            projectile->age = 0;
        }
    } else {
        if (projectile->age < state->lifetime)
            projectile->age = state->lifetime;
    }
}

void SlotActor_ResetOnFlags(struct ContactCollider *signal, struct ContactCollider *state,
                            struct SceneProjectile *actor)
{
    if (signal->statRewardFlags & 7) {
        Sound_PlaySong(0x170);
        actor->launch.contactCount = 0;
        actor->params.direction -= 0x80;
        actor->age = 0;
        actor->params.trackingFrames = 0;
        actor->params.speed = 0x24;
        state->contactFlags = 1;
        actor->attack.contactFlags = 1;
    }
}

void Projectile_InitParameters(struct SceneProjectileParams *);

void Sprite_InitUnlinked(void *, void *, s32);

void Collider_SetCallback(void *, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void Collider_SetBodyProperties(void *, s32, s32, s32);

static inline void ClearProjectileScratch(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}

void SceneProjectile_InitSlot(void *pool, struct SceneProjectile *projectile, s32 index)
{
    struct SceneProjectileColliderVector size, offset;
    struct SceneProjectileParams *config = &projectile->params;
    struct ContactCollider *collider;
    Projectile_InitParameters(config);
    projectile->age = 0;
    ClearProjectileScratch(&projectile->velocity);
    AnimData_Load(&projectile->graphics, config->graphicsId);
    Sprite_InitUnlinked(&projectile->sprite, &projectile->graphics, 1);
    collider = &projectile->attack;
    Collider_Init(collider, 0, 0x2001, 0, 16, &projectile->params.bounds,
                  &projectile->params.offset);
    Collider_SetCallback(collider, SceneProjectile_RespondToContact, projectile);
    Collider_SetAttackProperties(collider, config->collisionProperties[2],
                                 config->collisionProperties[3], config->collisionProperties[0],
                                 config->collisionProperties[1], config->collisionProperties[4]);
    collider = &projectile->body;
    size.x = 12;
    size.y = 120;
    size.z = 12;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, SlotActor_ResetOnFlags, projectile);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
    collider->flags |= 4;
}

void Sprite_Remove(void *);

void SceneProjectile_ReleaseSlot(struct SceneProjectilePool *pool,
                                 struct SceneProjectile *projectile, u32 index)
{
    u32 m;
    projectile->body.flags |= 4;
    projectile->sprite.flags = (projectile->sprite.flags & ~2) | (m = 1);
    Sprite_Remove(&projectile->sprite.flags);
    pool->used &= ~(m << index);
}

extern const s16 gUnk_086149C4[];

s32 Div(s32, s32);

s32 Ratatosk_GetSelectedPointIndex(void);

struct ContactPosition *Ratatosk_GetPosition(void);

struct ContactPosition *Player_GetEffectivePosition(s32);

void RisingActorManager_BeginRaise(s32);

s32 Math_Atan2(s32, s32);

u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline u16 AttackExtent(struct OrbitEffectSprite *sprite)
{
    return ((struct SceneProjectile *)((u8 *)sprite - (u32) &
                                       ((struct SceneProjectile *)0)->sprite))
        ->attack.contactFlags;
}

static inline void Shrink(struct SceneProjectileLaunch *launch, s16 *scale,
                          struct OrbitEffectSprite *sprite)
{
    launch->scale -= 2;
    if (launch->scale < 0)
        launch->scale = 0;
    sprite->scaleX = *scale;
    sprite->scaleY = *scale;
    sprite->position.y -= 4;
}

static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline void Polar(struct ContactPosition *out, struct SceneProjectileParams *control)
{
    s32 angle = control->direction;
    s32 speed = control->speed;
    out->x = Truncate(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Truncate(speed * gUnk_086149C4[angle & 255]);
}

static inline s32 CellIndex(struct ContactPosition *v)
{
    s32 x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *Cell(s32 i)
{
    u8 *cell = TerrainModifiers_FindCellOverride(i, 1);
    if (cell)
        return cell + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

static inline void StepAnimation(struct SceneProjectile *cursor, void *obj)
{
    struct OrbitEffectSprite *sprite = obj;
    struct DisplaySequenceState *state = &cursor->sequence;
    const u16 *command;
    u32 flags;
    {
        u32 offset = DisplaySequence_GetIndex(&cursor->sequence) << 1;
        const u16 *table = cursor->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first = 1;
        u32 bit = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        flags = state->flags;
        if (flags & 4) {
            if (state->index == 0) {
                u32 hold = 8;
                hold &= flags;
                if (hold)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else
                state->index--;
        } else {
            if (++state->index >= state->frameCount) {
                u32 hold = 8;
                hold &= flags;
                if ((u8)hold) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else
                    state->index = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 weight = state->frameWeight;
            s32 speed = state->speed;
            speed *= weight;
            speed >>= 6;
            state->duration = speed;
            if (!(u8)speed)
                state->duration = 1;
        }
    }
}

// confidence=low: updates ten scripted projectile slots and their launch delays.

s32 SceneProjectile_UpdatePool(struct SceneProjectilePool *pool)
{
    struct SceneProjectile *projectile;
    s32 i;
    struct OrbitEffectSprite *geometry;
    struct ContactCollider *collider;
    struct OrbitEffectSprite *animationSprite;
    struct ContactPosition *terrainPosition;
    for (projectile = pool->slots, i = 0; i <= 9; i++, projectile++) {
        struct OrbitEffectSprite *sprite = &projectile->sprite;
        struct SceneProjectileLaunch *launch = &projectile->launch;
        u32 active = pool->used & (1 << i);
        terrainPosition = (struct ContactPosition *)&projectile->sprite.position;
        if (active) {
            struct SceneProjectileParams *control;
            struct ContactPosition *position, *d;
            s32 result, targetIndex;
            geometry = sprite;
            collider = &projectile->attack;
            control = &projectile->params;
            result = sub_08249240(projectile, (void *)i, control->callback);
            animationSprite = sprite;
            if (result == 1) {
                SceneProjectile_ReleaseSlot(pool, projectile, i);
            } else {
                sprite->flags &= ~0x200;
                if (control->contactTimer > -10) {
                    {
                        u32 mask = ~2;
                        projectile->flags &= mask;
                    }
                    if (control->contactTimer == 120) {
                        projectile->flags |= 1;
                        control->speed = 0;
                    }
                    if (control->contactTimer > Div(120, 2)) {
                        Shrink(launch, &projectile->launch.scale, sprite);
                    }
                    if (control->contactTimer == Div(120, 2)) {
                        if (AttackExtent(sprite) == 16)
                            projectile->age = control->lifetime;
                        else if ((targetIndex = Ratatosk_GetSelectedPointIndex()) >= 0) {
                            sprite->position = *SunOrb_GetPosition(targetIndex);
                            sprite->position.y += 128;
                            launch->scale = 64;
                            sprite->scaleX = 64;
                            sprite->scaleY = 64;
                            control->trackingFrames = 120;
                            RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
                        }
                    }
                    animationSprite->flags |= 0x200;
                    if ((u16)(control->contactTimer - 31) <= 58)
                        animationSprite->flags |= 1;
                    else if (control->contactTimer <= 29)
                        animationSprite->flags &= ~1;
                    if (control->contactTimer == 1) {
                        if (AttackExtent(sprite) == 16) {
                            struct ContactPosition *target = Player_GetEffectivePosition(0);
                            s32 direction = Math_Atan2(target->x - sprite->position.x,
                                                       target->z - sprite->position.z);
                            u32 random;
                            s32 aim;
                            projectile->params.direction = direction;
                            gRandomIndex = (gRandomIndex + 1) & 1023;
                            random = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
                            aim = direction + ((random & 15) - 8);
                            projectile->params.direction = aim;
                            control->trackingFrames = 0;
                            control->speed = 18;
                        } else {
                            struct ContactPosition *target = Ratatosk_GetPosition();
                            projectile->params.direction = Math_Atan2(
                                target->x - sprite->position.x, target->z - sprite->position.z);
                            projectile->params.target = Ratatosk_GetPosition();
                            control->speed = launch->speed;
                            control->turnSpeed = launch->mode;
                        }
                        projectile->flags |= 1;
                        projectile->age = 0;
                    }
                    control->contactTimer--;
                }
                if (projectile->flags & 1)
                    Polar(&projectile->velocity, control);
                d = &projectile->velocity;
                position = (struct ContactPosition *)&geometry->position;
                geometry->position.x += projectile->velocity.x;
                position->y += d->y;
                position->z += d->z;
                if (control->trackingEnabled) {
                    s32 dx = control->target->x - sprite->position.x,
                        dz = control->target->z - sprite->position.z;
                    if (dx * dx + dz * dz <= 1023 || (u32)((*Cell(CellIndex(terrainPosition)) & 15)
                                                           << 8) > (u32)sprite->position.y) {
                        SceneProjectile_ReleaseSlot(pool, projectile, i);
                        continue;
                    }
                } else {
                    u32 mask;
                    projectile->body.flags |= 4;
                    mask = 2;
                    if (projectile->flags & mask) {
                        Collider_SetPosition(collider, position, 0);
                        Collider_Refresh(collider);
                        if (projectile->params.mode == 2) {
                            collider = &projectile->body;
                            Collider_SetPosition(collider, position, 0);
                            collider->flags &= 0xFFFB;
                        }
                    }
                }
                if (projectile->params.script)
                    StepAnimation(projectile, animationSprite);
                projectile->age++;
            }
        }
        terrainPosition = (struct ContactPosition *)&projectile->sprite.position;
    }
    return 0;
}

u32 SceneProjectile_OnPoolDestroy(struct SceneProjectilePool *pool)
{
    struct SceneProjectile *projectile = pool->slots;
    s32 i = 0;
    do {
        if (pool->used & (1 << i))
            SceneProjectile_ReleaseSlot(pool, projectile, i);
        i++;
        projectile++;
    } while (i <= 9);
    return gSceneProjectilePool = 0;
}

s32 SceneProjectile_InitPool(struct SceneProjectilePool *pool)
{
    struct SceneProjectile *projectile;
    s32 i;
    gSceneProjectilePool = pool;
    pool->used = 0;
    projectile = pool->slots;
    i = 0;
    do {
        SceneProjectile_InitSlot(pool, projectile, i);
        i++;
        projectile++;
    } while (i <= 9);
    return 0;
}

void *SceneProjectile_CreatePool(void)
{
    void *pool = Task_Create(8, sizeof(struct SceneProjectilePool));
    if (pool) {
        Task_SetCallbacks(pool, SceneProjectile_UpdatePool, SceneProjectile_OnPoolDestroy);
        if (SceneProjectile_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}
