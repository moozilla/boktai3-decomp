#include "gba/gba.h"
#include "engine/video/palette_pulse.h"
#include "ui/game_over/particle_pool.h"

void Palette_UpdateHighlightPulse(u16 *out, u8 *counter)
{
    u32 frame = *counter;
    u32 color;

    if (frame < 10)
        color = 31;
    else if (frame < 18)
        color = 27;
    else if (frame < 26)
        color = 18;
    else if (frame < 34)
        color = 10;
    else if (frame < 42)
        color = 18;
    else
        color = 27;
    (*counter)++;
    if (*counter >= 50)
        *counter = 0;
    *out = color;
}

extern const s16 gUnk_086149C4[];

static inline s32 TruncatePoolOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

void SpiralParticles_UpdateParticle(struct ParticlePool *pool, struct PoolParticle *particle)
{
    particle->tail.control.angle += particle->tail.control.angularSpeed;
    particle->tail.position.axis.x +=
        TruncatePoolOffset(particle->tail.control.radius * gUnk_086149C4[(particle->tail.control.angle + 64) & 255]);
    particle->tail.position.axis.z +=
        TruncatePoolOffset(particle->tail.control.radius * gUnk_086149C4[particle->tail.control.angle]);
    if (pool->frame >= particle->tail.control.expandDelay) {
        if (!(pool->frame & 7) && particle->tail.control.angularSpeed > 2)
            particle->tail.control.angularSpeed--;
        particle->tail.control.radius += 6;
        particle->tail.position.axis.y += particle->tail.control.riseSpeed;
    } else {
        particle->tail.control.radius++;
    }
    switch (particle->tail.control.kind) {
        case 0:
            if (particle->tail.control.animationPhase == 0)
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 2);
            else if (particle->tail.control.animationPhase == 4)
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 3);
            break;
        case 1:
            if (particle->tail.control.animationPhase == 0)
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 4);
            else if (particle->tail.control.animationPhase == 4)
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 5);
            break;
    }
    if (++particle->tail.control.animationPhase > 7)
        particle->tail.control.animationPhase = 0;
}

void ParticlePool_UpdateLinearParticle(struct ParticlePool *pool, struct PoolParticle *particle)
{
    particle->tail.position.axis.x += particle->tail.control.velocityX;
    particle->tail.position.axis.z += particle->tail.control.velocityZ;
    particle->tail.control.animationPhase++;
    if (particle->tail.control.animationPhase == 5) {
        switch (particle->tail.control.kind) {
            case 0:
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 3);
                break;
            case 1:
                SpriteDrawable_SelectTileFrame(particle, pool->resource, 5);
                break;
        }
    }
}

void ParticlePool_SpawnJitteredParticle(struct ParticlePool *pool, struct SceneVec *position,
                                        s32 angle, u32 kind)
{
    s32 i;
    union ParticleRandomTableSource randomSource;
    const u16 *table;

    for (i = 0; i < 16; i++) {
        if (!pool->particles[i].tail.control.active)
            break;
    }
    if (i < 16) {
        struct PoolParticle *particle = &pool->particles[i];

        particle->flags &= ~1;
        particle->tail.position.signedAxis = *position;
        {
            u32 *randomIndex;
            u32 range;
            u32 index;
            randomSource.halves[0] = (u16)RANDOM_TABLE_ADDR;
            randomSource.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&randomSource);
            randomIndex = &gRandomIndex;
            index = *randomIndex;
            index++;
            index &= 0x3FF;
            {
                u32 random = table[index];
                s32 x = (u16)particle->tail.position.signedAxis.x;
                x -= 63;
                range = 0x7F;
                random &= range;
                particle->tail.position.signedAxis.x = x + random;
            }
            index = (index + 1) & 0x3FF;
            *randomIndex = index;
            {
                u32 random = ParticleRandom_ReadCursor(table, &index);
                s32 z = (u16)particle->tail.position.signedAxis.z;
                z -= 63;
                random &= range;
                particle->tail.position.signedAxis.z = z + random;
            }
        }
        particle->tail.position.signedAxis.y += 128;
        if (kind != 0) {
            if (kind == 1) {
                if ((i & 1) == 0)
                    SpriteDrawable_SelectTileFrame(particle, pool->resource, 4);
                else
                    SpriteDrawable_SelectTileFrame(particle, pool->resource, 5);
            }
        } else if ((i & 1) == 0) {
            SpriteDrawable_SelectTileFrame(particle, pool->resource, 2);
        } else {
            SpriteDrawable_SelectTileFrame(particle, pool->resource, 3);
        }
        particle->tail.control.velocityX =
            TruncatePoolOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        particle->tail.control.velocityZ = TruncatePoolOffset(6 * gUnk_086149C4[angle & 255]);
        particle->tail.control.animationPhase = 0;
        particle->tail.control.state = 1;
        {
            u32 index = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = *(u16 *)((index << 1) + RANDOM_TABLE_ADDR);
                particle->tail.control.lifetime = (random & 7) + 8;
            }
        }
        particle->tail.control.active = 1;
        particle->tail.control.kind = kind;
    }
}

void SpriteParticles_UpdateEntries(struct ParticlePool *pool)
{
    struct PoolParticle *particle = pool->particles;
    s32 i;

    for (i = 0; i < 16; i++) {
        struct ParticlePoolControl *control = &pool->particles[i].tail.control;

        if (control->active) {
            if (--control->lifetime == 0) {
                particle->flags |= 1;
                control->active = 0;
            } else if (control->state == 0) {
                SpiralParticles_UpdateParticle(pool, particle);
            } else {
                ParticlePool_UpdateLinearParticle(pool, particle);
            }
        }
        particle++;
    }
    pool->frame++;
}

s32 ParticlePool_OnUpdate(struct ParticlePool *pool)
{
    SpriteParticles_UpdateEntries(pool);
    return 0;
}

s32 ParticlePool_OnDestroy(struct ParticlePool *pool)
{
    s32 i;

    for (i = 0; i < 16; i++)
        Effect_Remove(&pool->particles[i]);
    return 0;
}

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void CollisionVolume_InitCollider(struct ParticlePool *p)
{
    struct ParticlePoolVolumeVector size, offset;
    void *collider = p->collider;
    size.x = 30000;
    size.y = 30000;
    size.z = 30000;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x3001, 0, 1, &size, &offset);
    Collider_SetAttackProperties(collider, 100, 50, 0, 0x40000, 50);
    Collider_SetCallback(collider, 0, p);
}

void ParticleSprite_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

void Effect_SetPalette(void *, u32);

static inline u32 RandomByte(void)
{
    u32 offset;
    u32 *index = &gRandomIndex;
    u32 next = *index + 1;
    *index = next & 1023;
    offset = *index * 2;
    return *(const u8 *)(offset + RANDOM_TABLE_ADDR);
}

static inline s32 RandomValue(const u16 *table, u32 *index, u32 mask)
{
    u32 offset;
    *index = (*index + 1) & mask;
    offset = *index * 2;
    return *(const u16 *)(offset + (u32)table);
}

/* Tentative name: ParticlePool_InitParticles. Kind is retained from the slot. */

void ParticlePool_InitParticles(struct ParticlePool *pool)
{
    s32 i;
    u32 *index;
    u32 mask;
    u32 base = RandomByte();
    s32 angleOffset;
    u32 zero;
    struct PoolParticle *sprite;
    struct PoolParticleTail *tail;
    pool->resource = AnimData_FindEntry(0x1C1E);
    i = 0;
    index = &gRandomIndex;
    mask = 1023;
    zero = 0;
    tail = &pool->particles[0].tail;
    sprite = pool->particles;
    angleOffset = 0;
    for (; i < 16; i++) {
        const u16 *table;
        ParticleSprite_Initialize(sprite, pool->resource, 1);
        Effect_SetOrigin(sprite, -4, -4);
        Effect_SetPalette(sprite, 1);
        table = (const u16 *)RANDOM_TABLE_ADDR;
        tail->control.angle = base + angleOffset + (RandomValue(table, index, mask) & 7);
        tail->control.angularSpeed = (RandomValue(table, index, mask) & 3) + 7;
        tail->control.radius = ((RandomValue(table, index, mask) >> 3) & 0x5F) + 0x60;
        tail->control.riseSpeed = ((i & 3) << 1) + 1;
        tail->control.expandDelay = (RandomValue(table, index, mask) & 0x1F) + 8;
        CpuFill32(zero, tail->position.words, 8);
        sprite->renderMode = 2;
        sprite->priority = 0xEC;
        {
            u8 phase = RandomValue(table, index, mask) & 7;
            tail->control.animationPhase = phase;
            switch (tail->control.kind) {
                case 0:
                    if (phase <= 3)
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 2);
                    else
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 3);
                    break;
                case 1:
                    if (phase <= 3)
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 4);
                    else
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 5);
                    break;
            }
        }
        tail->control.animationPhase = zero;
        tail->control.state = zero;
        tail->control.lifetime = zero;
        tail->control.active = zero;
        tail = (struct PoolParticleTail *)((u8 *)tail + sizeof(*sprite));
        table = (const u16 *)RANDOM_TABLE_ADDR;
        sprite++;
        angleOffset += 16;
    }
}

s32 ParticlePool_Init(struct ParticlePool *p)
{
    CollisionVolume_InitCollider(p);
    ParticlePool_InitParticles(p);
    return 0;
}

struct ParticlePool *ParticlePool_Create(void)
{
    struct ParticlePool *p = Task_Create(10, 0x43c);
    if (p == 0)
        return 0;
    Task_SetCallbacks(p, ParticlePool_OnUpdate, ParticlePool_OnDestroy);
    if (ParticlePool_Init(p) < 0) {
        Task_Kill(p);
        return 0;
    }
    return p;
}

void ParticlePool_Destroy(struct ParticlePool *p)
{
    Task_Kill(p);
}

void ParticlePool_EmitTriple(struct ParticlePool *pool, struct SceneVec *position, s32 angle,
                             u32 kind)
{
    s32 i;
    struct ParticlePool *saved;
    saved = pool;
    for (i = 2; i >= 0; i--) {
        ParticlePool_SpawnJitteredParticle(saved, position, angle, kind);
    }
}
