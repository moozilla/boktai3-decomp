// MATCHING_CONCESSION: register-pinning
#include "directional_particles.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
extern u32 gUnk_03005308;
s32 DirectionalParticle_Update(void *, struct DirectionalParticle *);
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
    u16 randomIndex;
    typeof(motion) c1;
    typeof(point) c2;
    typeof(point) c3;
    if (!particle)
        return -1;
    motion = &particle->motion;
    c1 = motion;
    c1->lifetime = (nextRandom() & 7) + 10;
    speed = (nextRandom() & 7) + 24;
    c1->velocity.x = scaleVelocity(gUnk_086149C4[(direction + 64) & 255] * speed);
    c1->velocity.y = 0;
    motion->velocity.z = scaleVelocity(gUnk_086149C4[direction & 255] * speed);
    point = &position;
    c3 = point;
    c2 = c3;
    asm("" : "+r"(c3));
    *c2 = *source->position;
    randomIndex = (gUnk_03005308 + 1) & 1023;
    c2->x = (c2->x - 64) + (((const u16 *)0x0203B400)[randomIndex] & 127);
    randomIndex = (randomIndex + 1) & 1023;
    {
        typeof(((const u16 *)0x0203B400)[randomIndex]) v2 = ((const u16 *)0x0203B400)[randomIndex];
        c3->y = (c3->y + 16) + (v2 & 31);
    }
    randomIndex = (randomIndex + 1) & 1023;
    gUnk_03005308 = randomIndex;
    {
        typeof(((const u16 *)0x0203B400)[randomIndex]) v1 = ((const u16 *)0x0203B400)[randomIndex];
        c3->z = (c3->z - 64) + (v1 & 127);
    }
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
