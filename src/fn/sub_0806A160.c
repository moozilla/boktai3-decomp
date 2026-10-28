#include "global.h"

#include "spark_particle.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
void Effect_SetPalette(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);

static inline u32 spark_0806A160_Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}

static inline s32 spark_0806A160_Spread(s32 range)
{
    return Mod(spark_0806A160_Random(), range) - range / 2;
}

#ifdef NONMATCHING
// fuzzy: 38.3%
// scope shape 204 (differences once low registers are set aside)
// confidence=low: emits up to count sparks into free slots of the 48-entry pool, placed around pos with optional per-axis spread, moving at angle and speed, with a palette and animation chosen by kind.
s32 SparkParticle_Spawn(const struct ContactPosition *pos, u8 count,
                        const struct ContactPosition *spread, s32 angle, s32 speed,
                        s32 verticalRange, s32 kind, u32 priority)
{
    struct SparkParticlePool *pool;
    struct SparkParticle *particle;
    s32 index;
    s32 sequence;

    pool = gSparkParticlePool;
    if (pool == NULL) {
        pool = SparkParticle_CreatePool();
        if (pool == NULL) {
            return 0;
        }
    }

    particle = pool->particles;
    for (index = 0; index < 48; index++, particle++) {
        if (particle->active != 0) {
            continue;
        }

        if (spread != NULL) {
            if (spread->x == 0) {
                particle->sprite.position.x = pos->x;
            } else {
                particle->sprite.position.x = pos->x + spark_0806A160_Spread(spread->x);
            }
            if (spread->y == 0) {
                particle->sprite.position.y = pos->y;
            } else {
                particle->sprite.position.y = pos->y + spark_0806A160_Spread(spread->y);
            }
            if (spread->z == 0) {
                particle->sprite.position.z = pos->z;
            } else {
                particle->sprite.position.z = pos->z + spark_0806A160_Spread(spread->z);
            }
        } else {
            particle->sprite.position.x = pos->x;
            particle->sprite.position.y = pos->y;
            particle->sprite.position.z = pos->z;
        }
        particle->sprite.flags &= ~1;
        particle->sprite.priority = priority;

        particle->active = 1;
        ZeroFill8(&particle->velocity);

        if (speed != 0) {
            s32 product;

            product = gUnk_086149C4[(angle + 0x40) & 0xFF] * speed;
            if (product < 0) {
                particle->velocity.x = -((-product) >> 12);
            } else {
                particle->velocity.x = product >> 12;
            }
            particle->velocity.y = 0;
            product = gUnk_086149C4[angle & 0xFF] * speed;
            if (product < 0) {
                particle->velocity.z = -((-product) >> 12);
            } else {
                particle->velocity.z = product >> 12;
            }
        }
        if (verticalRange == 0) {
            particle->velocity.y = Mod(spark_0806A160_Random(), 0x20);
        } else {
            particle->velocity.y = Mod(spark_0806A160_Random(), verticalRange);
        }

        if (kind == 0 || kind == 1) {
            sequence = 0xF;
            Effect_SetPalette(&particle->sprite, 1);
        } else if (kind == 2) {
            sequence = 0x10;
            Effect_SetPalette(&particle->sprite, 0);
        } else if (kind == 3) {
            sequence = 0x11;
            Effect_SetPalette(&particle->sprite, 0);
        } else {
            sequence = 0xF;
            Effect_SetPalette(&particle->sprite, 0);
        }
        Anim_SetSequence(particle, pool->sequenceResource, sequence, 0, 0);
        pool->activeCount++;

        count--;
        if (count == 0) {
            return 1;
        }
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", SparkParticle_Spawn);
#endif
