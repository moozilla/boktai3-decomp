#include "particle_pool.h"

void Palette_UpdateHighlightPulse(u16 *out, u8 *c)
{
    u32 v = *c;
    u32 r = 0x1f;
    if (v > 9) {
        r = 0x1b;
        if (v > 0x11) {
            r = 0x12;
            if (v > 0x19) {
                r = 0xa;
                if (v > 0x21) {
                    r = 0x1b;
                    if (v <= 0x29)
                        r = 0x12;
                }
            }
        }
    }
    *c = *c + 1;
    if (*c > 0x31)
        *c = 0;
    *out = r;
}

extern const s16 gUnk_086149C4[];

void SpiralParticles_UpdateParticle(struct ParticlePool *p, struct PoolParticle *s)
{
    s32 delta;
    s32 product2;
    s32 product;
    s32 deltaZ;
    s->tail.control.angle += s->tail.control.angularSpeed;
    product2 = s->tail.control.radius * gUnk_086149C4[(s->tail.control.angle + 0x40) & 0xff];
    if (product2 >= 0)
        delta = product2 >> 12;
    else
        delta = -(-product2 >> 12);
    s->tail.position.axis.x += delta;
    product = s->tail.control.radius * gUnk_086149C4[s->tail.control.angle];
    if (product >= 0)
        deltaZ = product >> 12;
    else
        deltaZ = -(-product >> 12);
    s->tail.position.axis.z += deltaZ;
    if (p->frame >= s->tail.control.expandDelay) {
        if (!(p->frame & 7) && s->tail.control.angularSpeed > 2)
            s->tail.control.angularSpeed--;
        s->tail.control.radius += 6;
        s->tail.position.axis.y += s->tail.control.riseSpeed;
    } else
        s->tail.control.radius++;
    switch (s->tail.control.kind) {
        case 0:
            if (s->tail.control.animationPhase == 0)
                SpriteDrawable_SelectTileFrame(s, p->resource, 2);
            else if (s->tail.control.animationPhase == 4)
                SpriteDrawable_SelectTileFrame(s, p->resource, 3);
            break;
        case 1:
            if (s->tail.control.animationPhase == 0)
                SpriteDrawable_SelectTileFrame(s, p->resource, 4);
            else if (s->tail.control.animationPhase == 4)
                SpriteDrawable_SelectTileFrame(s, p->resource, 5);
            break;
    }
    if (++s->tail.control.animationPhase > 7)
        s->tail.control.animationPhase = 0;
}

void ParticlePool_UpdateLinearParticle(struct ParticlePool *pool, struct PoolParticle *particle)
{
    {
        u16 x = particle->tail.control.velocityX;
        u16 y = particle->tail.position.axis.x;
        particle->tail.position.axis.x = x + y;
    }
    {
        u16 x = particle->tail.control.velocityZ;
        u16 y = particle->tail.position.axis.z;
        particle->tail.position.axis.z = x + y;
    }
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

static inline s32 TruncatePoolOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline u16 ReadParticleSample(const u16 *table, const u32 *index)
{
    return table[*index];
}

void ParticlePool_SpawnJitteredParticle(struct ParticlePool *inPool, struct SceneVec *position,
                                        s32 angle, u32 kind)
{
    s32 i;
    struct PoolParticle *p;
    union ParticleRandomTableSource randomSource;
    const u16 *table;
    for (i = 0; i < 16; i++) {
        if (!inPool->particles[i].tail.control.active) {
            break;
        }
    }

    if (i < 16) {
        {

            u32 *randomIndex;
            u32 range;
            u32 mask;
            u32 index;
            p = &inPool->particles[i];
            p->flags &= ~1;
            p->tail.position.signedAxis = *position;
            randomSource.halves[0] = (u16)RANDOM_TABLE_ADDR;
            randomSource.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&randomSource);
            randomIndex = &gRandomIndex;
            index = *randomIndex;
            index++;
            mask = 0x3FF;
            index &= mask;
            {
                u32 random = table[index];
                s32 x = (u16)p->tail.position.signedAxis.x;
                x -= 63;
                range = 0x7F;
                random &= range;
                p->tail.position.signedAxis.x = x + random;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            {
                u32 random = ReadParticleSample(table, &index);
                s32 z = (u16)p->tail.position.signedAxis.z;
                z -= 63;
                random &= range;
                p->tail.position.signedAxis.z = z + random;
            }
        }
        p->tail.position.signedAxis.y += 128;
        if (kind != 0) {
            if (kind == 1) {
                if ((i & kind) == 0)
                    SpriteDrawable_SelectTileFrame(p, inPool->resource, 4);
                else
                    SpriteDrawable_SelectTileFrame(p, inPool->resource, 5);
            }
        } else if ((i & 1) == 0)
            SpriteDrawable_SelectTileFrame(p, inPool->resource, 2);
        else
            SpriteDrawable_SelectTileFrame(p, inPool->resource, 3);
        p->tail.control.velocityX = TruncatePoolOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        p->tail.control.velocityZ = TruncatePoolOffset(6 * gUnk_086149C4[angle & 255]);
        p->tail.control.animationPhase = 0;
        p->tail.control.state = 1;
        {
            u32 index = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = *(u16 *)((index << 1) + RANDOM_TABLE_ADDR);
                u32 range = 7;
                p->tail.control.lifetime = (random & range) + 8;
            }
        }
        p->tail.control.active = 1;
        p->tail.control.kind = kind;
    }
}

void SpriteParticles_UpdateEntries(struct ParticlePool *p)
{
    struct PoolParticle *entry = p->particles;
    struct ParticlePoolControl *control = &p->particles[0].tail.control;
    int i = 15;
    do {
        if (control->active) {
            if (--control->lifetime == 0) {
                entry->flags |= 1;
                control->active = 0;
            } else if (control->state == 0) {
                SpiralParticles_UpdateParticle(p, entry);
            } else {
                ParticlePool_UpdateLinearParticle(p, entry);
            }
        }
        entry++;
        control = (void *)((u8 *)control + sizeof(*entry));
    } while (--i >= 0);
    p->frame++;
}

s32 ParticlePool_OnUpdate(struct ParticlePool *pool)
{
    SpriteParticles_UpdateEntries(pool);
    return 0;
}

s32 ParticlePool_OnDestroy(struct ParticlePool *pool)
{
    struct PoolParticle *particle = pool->particles;
    s32 remaining = 15;
    do {
        Effect_Remove(particle);
        particle++;
        remaining--;
    } while (remaining >= 0);
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
    u32 mask = 1023;
    *index = next & mask;
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
        {
            vu32 fill = zero;
            CpuSet((const void *)&fill, tail->position.words, 0x05000002);
        }
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
