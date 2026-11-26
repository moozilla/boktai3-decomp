#include "global.h"
#include "engine/engine.h"
#include "effects/particles/particle_group.h"
#include "engine/ram.h"
// confidence=medium: fills a particle group with randomized polar velocities.
#define ParticleGroup_SpawnRadial sub_080148CC
extern u32 gUnk_08604B90[];
static inline int CellIndex(struct ContactPosition *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0xffff;
    return gTerrainMap->rows[z] + x;
}
struct ParticleGroupManager *ParticleManager_Create(void *, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, s32);
static inline s32 Random(u16 *table, u32 mask, u32 *rng)
{
    u32 r = (*rng + 1) & mask;
    u16 sample;
    *rng = r;
    sample = table[r];
    return sample;
}
static inline void ClearFlag(u32 *p, u32 mask)
{
    u32 v = *p;
    v &= ~mask;
    *p = v;
}
static inline s32 FixedMultiply(s32 a, s32 b)
{
    s32 value = a * b;
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Direction(struct ContactPosition *cursor, s32 baseDirection, s32 offset,
                             s32 speed)
{
    struct ContactPosition *out = cursor;
    s32 angle = baseDirection + offset;
    out->x = FixedMultiply(gUnk_086149C4[(angle + 64) & 255], speed);
    out->y = 0;
    out->z = FixedMultiply(gUnk_086149C4[angle & 255], speed);
}
static inline s32 ReadScalar(const s32 *value)
{
    return *value;
}
static inline struct ContactPosition *ReadOrigin(struct ContactPosition *const *value)
{
    return *value;
}
static inline s32 SpawnInline(s32 count, s32 options, struct ContactPosition *position, s32 angle,
                              s32 angleRange, s32 speed, s32 speedRange, s32 vertical,
                              s32 verticalRange, s32 lifetime, s32 lifetimeMask)
{
    struct ParticleGroupManager *m = gParticleGroupManager;
    struct ParticleGroup *g;
    struct ParticleGroup *pool;
    u32 *rng;
    s32 priority;
    s32 j;
    s32 halfRange;
    s32 palette;
    s32 angleOffset;
    s32 i;
    struct ParticleGroupGraphics *resource;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    halfRange = ReadScalar(&options);
    priority = halfRange >> 8;
    options &= 255;
    palette = 8;
    if (ReadScalar(&count) > palette)
        count = palette;
    if (options <= 20) {
        resource = m->small;
        palette = resource->paletteBank;
    } else {
        resource = m->large;
        palette = resource->paletteBank + 1;
    }
    halfRange = angleRange >> 1;
    j = 0;
    rng = &gRandomIndex;
    pool = m->groups;
    g = m->groups;
    for (; j < 6; g++, pool++, j++) {
        if (!g->active) {
            g->active = 1;
            g->type = *((u8 *)&options);
            g->count = *((u8 *)&count);
            if (priority > 0)
                g->priority = priority - 1;
            else {
                g->priority = 2;
            }
            i = 0;
            angleOffset = count;
            if (i < angleOffset) {
                u32 randomMask = 0x3ff;
                u16 *table = gRandomTable;
                struct ContactPosition *vector = &pool->particles[0].velocity;
                struct ParticleGroupParticle *p;
                u8 *sprite;
                s32 normalizedAngle;
                p = pool->particles;
                sprite = (u8 *)&pool->particles[0].flags;
                normalizedAngle = ReadScalar(&angle);
                normalizedAngle &= 255;
                angleOffset = normalizedAngle * 2;
                i = count;
                do {
                    p->active = 1;
                    p->unknown1 = 1;
                    p->timer = 0;
                    p->lifetime = lifetime + (Random(table, randomMask, rng) & lifetimeMask);
                    p->cell = CellIndex(ReadOrigin(&position));
                    p->height = position->y;
                    {
                        s32 magnitude;
                        if (speedRange) {
                            s32 r = Random(table, randomMask, rng);
                            magnitude = speed + Mod(r, speedRange);
                        } else
                            magnitude = speed;
                        if (angleRange) {
                            s32 r = Random(table, randomMask, rng);
                            s32 offset = Mod(r, angleRange);
                            s32 baseDirection = angle - halfRange;
                            Direction(vector, baseDirection, offset, magnitude);
                        } else {
                            struct ContactPosition *out = vector;
                            out->x = FixedMultiply(gUnk_086149C4[(angle + 64) & 255], magnitude);
                            out->y = 0;
                            out->z = FixedMultiply(*((s16 *)(((u8 *)gUnk_086149C4) + angleOffset)),
                                                   magnitude);
                        }
                    }
                    if (verticalRange) {
                        s32 r = Random(table, randomMask, rng);
                        p->velocity.y = vertical + Mod(r, verticalRange);
                    } else
                        p->velocity.y = vertical;
                    if ((Random(table, randomMask, rng) & 15) <= 7)
                        p->flags |= 4;
                    else
                        p->flags &= ~4;
                    ClearFlag(&p->flags, 1);
                    p->position = *position;
                    p->palette = *((u8 *)&palette);
                    p->tile = resource->tileBase + gUnk_08604B90[options];
                    p->priority = g->priority;
                    sub_08217E40(sprite, 0);
                    vector = (void *)((u8 *)vector + sizeof(*p));
                    p++;
                    sprite += sizeof(*p);
                } while (--i);
                i = count;
            }
            if (i < 8) {
                u32 inactive = 0, hidden = 1;
                u32 offset = i * sizeof(struct ParticleGroupParticle);
                struct ParticleGroupParticle *p =
                    (struct ParticleGroupParticle *)(offset + (u32)pool->particles);
                do {
                    p->active = inactive;
                    p->flags |= hidden;
                    p++;
                    i++;
                } while (i < 8);
            }
            break;
        }
    }

    return 0;
}

s32 ParticleGroup_SpawnRadial(s32 count, s32 options, struct ContactPosition *position, s32 angle,
                              s32 angleRange, s32 speed, s32 speedRange, s32 vertical,
                              s32 verticalRange, s32 lifetime, s32 lifetimeMask)
{
    return SpawnInline(count, options, position, angle, angleRange, speed, speedRange, vertical,
                       verticalRange, lifetime, lifetimeMask);
}
