#include "global.h"
#include "engine.h"
#include "scene_vec.h"
#define SparkleEmitter_EmitParticle sub_081C29A4
#ifdef NONMATCHING
// fuzzy: 91.6%
// Remaining differences: RNG load/store order and register allocation around emission jitter.
struct Particle {
    u32 flags;
    u8 pad4[0x18];
    struct SceneVec pos;
    u8 pad24[8];
    struct SceneVec velocity;
    u32 age;
};
struct Emitter {
    u8 pad[0x774];
    s16 angle, speed;
    s32 count;
};
extern s16 gUnk_03005418[];
struct Particle *ParticlePool_FindFreeEntry(void *);
static inline s32 fixed(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline s32 randomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline s32 randomAngle(s32 random, s32 base, s32 mask)
{
    return ((random >> 4) & mask) + base;
}
static inline void screenDirection(struct SceneVec *out, s32 angle, s32 speed)
{
    out->x = fixed(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = fixed(speed * gUnk_086149C4[angle]);
    out->z = 0;
}
static inline s32 half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void screenToWorld(struct SceneVec *out, struct SceneVec *screen)
{
    s32 x = screen->x + gUnk_03005418[0] - 120;
    s32 y = screen->y + gUnk_03005418[1] - 80;
    out->x = half(((x + y) << 5) / 3 + 1024);
    out->y = 1024;
    out->z = half(((y - x) << 5) / 3 + 1024);
}
static inline void worldDirection(struct SceneVec *out, s32 baseAngle, s32 angleBits, s32 baseSpeed,
                                  s32 speedBits)
{
    s32 angle = baseAngle + angleBits;
    s32 speed = baseSpeed + speedBits;
    angle -= 8;
    out->x = fixed(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = fixed(speed * gUnk_086149C4[angle & 255]);
}
void SparkleEmitter_EmitParticle(struct Emitter *p)
{
    struct Particle *particle;
    struct SceneVec offset;
    s32 angle, speed;
    if (p->speed <= 1)
        return;
    particle = ParticlePool_FindFreeEntry(p);
    if (!particle)
        return;
    p->count++;
    particle->flags &= ~1;
    angle = (randomAngle(randomValue(), p->angle, 63) + 128) & 255;
    screenDirection(&offset, angle, 150);
    offset.x += 120;
    offset.y += 80;
    screenToWorld(&particle->pos, &offset);
    {
        s32 r = randomValue();
        s32 a = p->angle;
        s32 bits = (r >> 4) & 15;
        s32 t = randomValue();
        s32 b = p->speed;
        s32 speedBits = (t >> 4) & 3;
        worldDirection(&particle->velocity, a, bits, b, speedBits);
    }
    particle->age = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C29A4);
#endif
