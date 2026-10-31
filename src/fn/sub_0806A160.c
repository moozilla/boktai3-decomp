#include "global.h"

#include "spark_particle.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Effect_SetPalette(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);

struct SparkParticleDrawView {
    struct SparkParticleSprite sprite;
    struct SparkParticleMotion motion;
};

static inline u32 SparkParticle_NextSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 SparkParticle_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}

static inline s32 SparkParticle_ScaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void SparkParticle_SetVelocity(struct ContactPosition *input, s32 angle, s32 speed)
{
    struct ContactPosition *velocity = input;
    velocity->x = SparkParticle_ScaleVelocity(gUnk_086149C4[(angle + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = SparkParticle_ScaleVelocity(gUnk_086149C4[angle & 255] * speed);
}

s32 SparkParticle_Spawn(const struct ContactPosition *pos, u8 requestedCount,
                        const struct ContactPosition *spread, s32 angleOrCount, s32 speed,
                        s32 verticalRange, s32 kind, u32 priority)
{
    s32 angle;
    struct SparkParticlePool *pool;
    s32 index;
    union {
        s32 angle;
        const u16 *table;
    } setup;
    struct SparkParticle *particle;
    struct SparkParticleMotion *motion;
    struct SparkParticleSprite *sprite;
    struct SparkParticleDrawView *projected;
    s16 sequence;
    u32 sampleMask;

    /* Capture the emission direction, then use the incoming word for the remaining count. */
    setup.angle = angleOrCount;
    angleOrCount = requestedCount;
    angle = setup.angle;
    angleOrCount = (u8)angleOrCount;
    pool = gSparkParticlePool;
    if (pool == NULL) {
        pool = SparkParticle_CreatePool();
        if (pool == NULL)
            return 0;
    }
    index = 0;
    setup.table = gRandomTable;
    motion = (struct SparkParticleMotion *)&pool->particles[0].velocity;
    sprite = &pool->particles[0].sprite;
    projected = (struct SparkParticleDrawView *)sprite;
    particle = pool->particles;
    for (; index < 48; index++) {
        if (motion->active)
            goto next;
        {
            if (spread != NULL) {
                if (spread->x == 0)
                    projected->sprite.position.x = pos->x;
                else {
                    s32 sample;
                    s32 offset;
                    u32 next = gRandomIndex + 1;
                    sampleMask = 1023;
                    next &= sampleMask;
                    gRandomIndex = next;
                    sample = setup.table[next];
                    offset = Mod(sample, spread->x);
                    projected->sprite.position.x = offset - SparkParticle_Half(spread->x) + pos->x;
                }
                if (spread->y == 0)
                    projected->sprite.position.y = pos->y;
                else {
                    s32 sample = SparkParticle_NextSample(setup.table);
                    s32 offset = Mod(sample, spread->y);
                    projected->sprite.position.y = offset - SparkParticle_Half(spread->y) + pos->y;
                }
                if (spread->z == 0)
                    projected->sprite.position.z = pos->z;
                else {
                    s32 sample = SparkParticle_NextSample(setup.table);
                    s32 offset = Mod(sample, spread->z);
                    projected->sprite.position.z = offset - SparkParticle_Half(spread->z) + pos->z;
                }
            } else {
                projected->sprite.position.x = pos->x;
                projected->sprite.position.y = pos->y;
                projected->sprite.position.z = pos->z;
            }
            sampleMask = 1023;
            projected->sprite.flags &= ~1;
            projected->sprite.priority = priority;
            motion->active = 1;
            ZeroFill8(&motion->velocity);
            if (speed)
                SparkParticle_SetVelocity(&motion->velocity, angle, speed);
            if (verticalRange == 0)
                projected->motion.velocity.y = Mod(SparkParticle_NextSample(setup.table), 32);
            else
                projected->motion.velocity.y =
                    Mod(SparkParticle_NextSample(setup.table), verticalRange);
            switch (kind) {
                case 0:
                    sequence = 15;
                    Effect_SetPalette(sprite, 1);
                    break;
                case 1:
                    sequence = 15;
                    Effect_SetPalette(sprite, 1);
                    break;
                case 2:
                    sequence = 16;
                    Effect_SetPalette(sprite, 0);
                    break;
                case 3:
                    sequence = 17;
                    Effect_SetPalette(sprite, 0);
                    break;
                default:
                    sequence = 15;
                    Effect_SetPalette(sprite, 0);
                    break;
            }
            Anim_SetSequence(particle, pool->sequenceResource, sequence, 0, 0);
            pool->activeCount++;
            angleOrCount = (u8)(angleOrCount - 1);
            if (angleOrCount == 0)
                break;
        }
    next:
        motion = (struct SparkParticleMotion *)((u8 *)motion + sizeof(*particle));
        projected = (struct SparkParticleDrawView *)((u8 *)projected + sizeof(*particle));
        sprite = (struct SparkParticleSprite *)((u8 *)sprite + sizeof(*particle));
        particle++;
    }
    return 1;
}
