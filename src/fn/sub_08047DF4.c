// MATCHING_CONCESSION: register-pinning
#include "directional_particles.h"
extern u32 gUnk_03005308;
s32 DirectionalParticle_Update(void *, struct DirectionalParticle *);
#ifdef NONMATCHING
// fuzzy: 53.3%
// Lifetime/speed/position behavior recovered; stack and register allocation remain different.
// The original also materializes an unused 0x0403B714 literal, not reproduced here.
static inline u32 nextRandom(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
}
static inline s32 scaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 DirectionalParticle_Spawn(struct DirectionalParticleSource *source, u32 direction, u32 frame)
{
    struct DirectionalParticle *particle = SpritePool_Allocate();
    struct DirectionalParticleMotion *motion;
    struct ContactPosition position;
    struct ContactPosition *point;
    s32 speed;
    u32 randomIndex;
    if (!particle)
        return -1;
    motion = &particle->motion;
    motion->lifetime = (nextRandom() & 7) + 10;
    speed = (nextRandom() & 7) + 24;
    motion->velocity.x = scaleVelocity(gUnk_086149C4[(direction + 64) & 255] * speed);
    motion->velocity.y = 0;
    motion->velocity.z = scaleVelocity(gUnk_086149C4[direction & 255] * speed);
    point = &position;
    asm("" : "+r"(point));
    *point = *source->position;
    randomIndex = (gUnk_03005308 + 1) & 1023;
    point->x = point->x - 64 + (((const u16 *)RANDOM_TABLE_ADDR)[randomIndex] & 127);
    randomIndex = (randomIndex + 1) & 1023;
    point->y = point->y + 16 + (((const u16 *)RANDOM_TABLE_ADDR)[randomIndex] & 31);
    randomIndex = (randomIndex + 1) & 1023;
    gUnk_03005308 = randomIndex;
    point->z = point->z - 64 + (((const u16 *)RANDOM_TABLE_ADDR)[randomIndex] & 127);
    particle->visible = 1;
    SpritePool_SetResource(particle, 0x4147);
    SpritePool_SetFrame(particle, frame);
    SpritePool_SetPalette(particle, 0x164);
    SpritePool_SetPosition(particle, point);
    SpritePool_Activate(particle, (void *)DirectionalParticle_Update, source);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", DirectionalParticle_Spawn);
#endif
