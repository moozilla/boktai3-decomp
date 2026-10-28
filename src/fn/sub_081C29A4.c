#include "global.h"
#include "engine.h"
#include "render.h"
#include "ram.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Particle {
    u32 flags;
    u8 pad4[0x18];
    struct Vec pos;
    u8 pad24[8];
    struct Vec vel;
    u32 age;
};
struct Emitter {
    u8 pad[0x774];
    s16 angle, speed;
    u32 count;
};
extern const s16 gUnk_086149C4[];
struct Particle *ParticlePool_FindFreeEntry(struct Emitter *);
static inline s32 random(void)
{
    const u16 *table = gRandomTable;
    u32 *indexPtr = &gRandomIndex;
    u32 index = *indexPtr + 1;
    u32 mask = 1023;
    index &= mask;
    *indexPtr = index;
    return table[index];
}
static inline s32 randomCircle(void)
{
    /* The shared random table resides in work RAM. */
    return ((const u16 *)0x0203B400)[gRandomIndex = (gRandomIndex + 1) & 1023];
}
static inline s32 scale12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline s32 half(s32 x)
{
    return x >= 0 ? x >> 1 : -((-x) >> 1);
}
static inline void circle(s32 angle, struct Vec *p, s32 radius)
{
    p->x = scale12(gUnk_086149C4[(angle + 64) & 255] * radius);
    p->y = scale12(gUnk_086149C4[angle] * radius);
    p->z = 0;
}
static inline void unproject(struct Vec *p, struct Vec *screen)
{
    s32 x = screen->x + ((s16 *)gCameraOrigin)[0] - 120;
    s32 y = screen->y + ((s16 *)gCameraOrigin)[1] - 80;
    p->x = half(((x + y) << 5) / 3 + 1024);
    p->y = 1024;
    p->z = half(((y - x) << 5) / 3 + 1024);
}
static inline void polar(struct Vec *p, s32 angle, s32 speed)
{
    p->x = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->y = 0;
    p->z = scale12(gUnk_086149C4[angle & 255] * speed);
}
void SparkleEmitter_EmitParticle(struct Emitter *input)
{
    struct Emitter *emitter = input;
    struct Particle *particle;
    struct Vec screen;
    s32 angle;
    s32 baseAngle;
    if (emitter->speed <= 1)
        return;
    particle = ParticlePool_FindFreeEntry(emitter);
    if (!particle)
        return;
    emitter->count++;
    particle->flags &= ~1;
    angle = randomCircle();
    baseAngle = emitter->angle;
    angle = ((angle >> 4) & 63) + baseAngle;
    circle((angle + 128) & 255, &screen, 150);
    screen.x += 120;
    screen.y += 80;
    unproject(&particle->pos, &screen);
    polar(&particle->vel, (emitter->angle + ((random() >> 4) & 15)) - 8,
          emitter->speed + ((random() >> 4) & 3));
    particle->age = 0;
}
