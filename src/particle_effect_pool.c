#include "global.h"
#include "ram.h"
#include "particle_effect_pool.h"

void ScriptedColliderActor_Create(struct ScriptedColliderPosition *position, u32 variant, u32 a, u32 b, u32 c,
                                  u32 d);

s32 ScriptedColliderPool_HasActorAtPosition(s16 *pos)
{
    struct ScriptedColliderPool *pool = gScriptedColliderManager;
    s32 i;

    if (pool == NULL)
        return 0;
    for (i = 0; i < 8; i++) {
        if ((u8)(pool->actors[i].state - 1) <= 1 && pool->actors[i].x == pos[0] &&
            pool->actors[i].y == pos[1] && pool->actors[i].z == pos[2])
            return 1;
    }
    return 0;
}

void ScriptedColliderPool_StopAll(void)
{
    struct ScriptedColliderPool *pool = gScriptedColliderManager;
    s32 i;

    if (pool != NULL) {
        for (i = 0; i < 8; i++) {
            if (pool->actors[i].state != 3 && pool->actors[i].state != 0) {
                pool->actors[i].state = 3;
                pool->actors[i].enteredState = 1;
            }
        }
    }
}

void ScriptedColliderActor_SpawnFixed(void)
{
    struct ScriptedColliderPosition v;
    v.x = 0x980;
    v.y = 0x200;
    v.z = 0x880;
    ScriptedColliderActor_Create(&v, 0x78, 0, 0, 0, 0);
}

void ScriptedColliderPool_RunIdleHook(void)
{
}

static inline u8 test(volatile u32 *p, s32 one, s32 i)
{
    s32 mask = one;
    mask <<= i;
    if (*p & mask)
        return 1;
    return 0;
}

struct SceneParticle *SceneMovingParticle_AllocateEntry_0812D670(struct SceneParticlePool *pool)
{
    s32 i = 0;
    volatile u32 *flags = &pool->occupied;
    s32 one = 1;
    s8 *kind = &pool->particles[0].slotIndex;
    struct SceneParticle *entry = pool->particles;
    do {
        if (test(flags, one, i)) {
            kind += 0x40;
            entry++;
            ++i;
        } else {
            s32 bit;
            bit = one;
            *(&bit) <<= i;
            *flags |= bit;
            *kind = i;
            return entry;
        }
    } while (i <= 25);
    return 0;
}

void SpriteDrawable_SelectTileFrame(struct SceneParticle *, void *, u32);

s32 SceneParticle_UpdateAlternatingFrame(struct SceneParticlePool *pool,
                                         struct SceneParticle *particle)
{
    if (particle->frameTimer == 0) {
        particle->frameTimer = particle->frameInterval;
        particle->framePhase = (particle->framePhase + 1) & 1;
        SpriteDrawable_SelectTileFrame(particle, pool->resource,
                                       particle->baseFrame + particle->framePhase);
    } else
        particle->frameTimer--;
    return 0;
}

s32 SceneParticle_UpdateAlternatingFrame(struct SceneParticlePool *, struct SceneParticle *);

void SceneParticle_UpdateMotion(struct SceneParticlePool *pool, struct SceneParticle *particle)
{
    if (particle->reset) {
        particle->flags &= ~1;
        particle->reset = 0;
    }
    particle->position.x = particle->position.x + particle->velocity.x;
    particle->position.y = particle->position.y + particle->velocity.y;
    SceneParticle_UpdateAlternatingFrame(pool, particle);
    if (particle->lifetime)
        particle->lifetime--;
    else {
        particle->state = 2;
        particle->reset = 1;
    }
}

void Effect_Remove(void *);

void SceneParticle_Remove(void *unused, struct SceneParticle *particle)
{
    if (particle->reset)
        particle->reset = 0;
    {
        struct SceneParticlePool *pool = gSceneParticlePool;
        s32 index = particle->slotIndex;
        pool->occupied &= ~(1 << index);
    }
    particle->state = 0;
    Effect_Remove(particle);
    particle->slotIndex = -1;
    particle->elapsed = 0;
    particle->lifetime = 0;
}

struct SceneParticle;

struct SceneParticleVector;

struct SceneParticle *SceneParticle_Create(struct SceneParticleVector *position,
                                           struct SceneParticleVector *velocity, u32 lifetime,
                                           u32 kind, u32 mode);

struct SceneParticle *SceneParticle_CreateDefaultMode(struct SceneParticleVector *position,
                                   struct SceneParticleVector *velocity, u32 lifetime, u32 kind)
{
    return SceneParticle_Create(position, velocity, lifetime, kind, 1);
}

struct SceneParticle *SceneParticle_CreateWithMode(struct SceneParticleVector *position,
                                   struct SceneParticleVector *velocity, u32 lifetime, u32 kind,
                                   u32 mode)
{
    return SceneParticle_Create(position, velocity, lifetime, kind, mode);
}

#define SceneParticle_Allocate   SceneMovingParticle_AllocateEntry_0812D670

#define SceneParticle_CreatePool ParticleEffectPool_GetOrCreate

struct SceneParticlePool *ParticleEffectPool_GetOrCreate(void);

struct SceneParticle *SceneMovingParticle_AllocateEntry_0812D670(struct SceneParticlePool *);

void ParticleSprite_Initialize(struct SceneParticle *, void *, u32);

void Effect_SetOrigin(struct SceneParticle *, s32, s32);

void SpriteDrawable_SelectTileFrame(struct SceneParticle *, void *, u32);

void Effect_SetPalette(struct SceneParticle *, u32);

struct SceneParticle *SceneParticle_Create(struct SceneParticleVector *position,
                                           struct SceneParticleVector *velocity, u32 lifetime,
                                           u32 kind, u32 mode)
{
    struct SceneParticlePool *pool = gSceneParticlePool;
    struct SceneParticle *p;
    u32 frame;
    if (!pool) {
        pool = ParticleEffectPool_GetOrCreate();
        if (!pool)
            return 0;
    }
    p = SceneMovingParticle_AllocateEntry_0812D670(pool);
    if (!p)
        return 0;
    p->state = 1;
    p->elapsed = 0;
    p->reset = 1;
    p->velocity = *velocity;
    p->lifetime = lifetime;
    p->delay = 30;
    p->frameTimer = 0;
    switch (kind) {
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
    p->framePhase = 0;
    p->baseFrame = frame << 1;
    p->frameInterval = 3;
    p->frameTimer = p->frameInterval;
    p->position = *position;
    ParticleSprite_Initialize(p, pool->resource, 0);
    Effect_SetOrigin(p, -4, -4);
    SpriteDrawable_SelectTileFrame(p, pool->resource, p->baseFrame);
    Effect_SetPalette(p, 1);
    p->flags |= 1;
    p->mode = mode;
    return p;
}

extern SceneParticleUpdate gUnk_08610808[];

u32 ParticleEffectPool_Update(struct SceneParticlePool *pool)
{
    struct SceneParticle *start = pool->particles;
    struct SceneParticle *entry;
    s32 i;
    u32 *occupied;
    u8 *state;
    SceneParticleUpdate *callbacks;
    if (!pool->occupied)
        return 0;
    i = 0;
    occupied = (u32 *)SceneParticlePool_SlotMask(pool);
    callbacks = gUnk_08610808;
    state = &pool->particles[0].state;
    entry = start;
    for (; i < 26; state += sizeof(*entry), entry++, i++) {
        if (SceneParticlePool_IsOccupied(occupied, i))
            callbacks[*state](pool, entry);
    }
    return 0;
}

u32 ParticleEffectPool_DestroyTask(struct SceneParticlePool *pool)
{
    s32 i = 0;
    volatile u32 *occupied = &pool->occupied;
    struct SceneParticle *particle = pool->particles;
    for (; i < 26; particle++, i++) {
        if (SceneParticlePool_IsOccupied(occupied, i))
            Effect_Remove(particle);
    }
    {
        u32 z = 0;
        gSceneParticlePool = z;
    }
    return 0;
}

u32 AnimData_FindEntry(u32);

s32 ParticleEffectPool_Init(struct SceneParticlePool *pool)
{
    pool->resource = (void *)AnimData_FindEntry(0x1c1e);
    pool->occupied = 0;
    gSceneParticlePool = pool;
    return 0;
}

u8 *Task_Create(u32, u32);

void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));

s32 ParticleEffectPool_Init(struct SceneParticlePool *);

void Task_Kill(u8 *);


struct SceneParticlePool *ParticleEffectPool_GetOrCreate(void)
{
    struct SceneParticlePool *p;
    if (gSceneParticlePool != 0)
        return gSceneParticlePool;
    p = (struct SceneParticlePool *)Task_Create(0xa, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, ParticleEffectPool_Update, ParticleEffectPool_DestroyTask);
        if (ParticleEffectPool_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void ParticleEffectPool_ResetSingleton(void)
{
    gSceneParticlePool = 0;
}
