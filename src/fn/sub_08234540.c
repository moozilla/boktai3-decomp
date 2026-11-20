#include "spiral_burst.h"

#include "monster_particle_random.h"
void SpiralBurst_InitParticles(struct SpiralBurst *burst)
{
    const u16 *table;
    s32 i;
    u8 phase;
    struct SpriteDrawable *sprite;
    struct SpiralParticleFrame *frame;
    {
        const u16 *top = gRandomTable;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        phase = *(const u8 *)(gRandomIndex * 2 + (u32)top);
    }
    burst->resource = AnimData_FindEntry(0x1C1E);
    i = 0;
    /* Form the table copy after CSE has finished tracking constants. */
    table = (const u16 *)((u32)gRandomTable & ~(phase >> 8));
    frame = &burst->particles[0].frame;
    sprite = (void *)burst->particles;
    for (; i < 8; i++) {
        ParticleSprite_Initialize(sprite, burst->resource, 0);
        Effect_SetOrigin(sprite, -4, -4);
        Effect_SetPalette(sprite, 1);
        frame->motion.angle = phase + (i << 5) + (ParticleRandom_ReadDirectSample(table) & 15);
        frame->motion.riseRate = ((i & 3) << 1) + 3;
        frame->motion.radius = (ParticleRandom_ReadDirectSample(table) & 31) + 64;
        frame->motion.riseDelay = (ParticleRandom_ReadDirectSample(table) & 15) + 12;
        frame->motion.angularSpeed = 7;
        {
            union {
                const s16 *table;
                s16 sample;
            } wave;
            s32 radius = frame->motion.radius;
            wave.table = gUnk_086149C4;
            wave.sample = wave.table[((frame->motion.angle + 64) & 255)];
            frame->motion.position.x =
                burst->origin.x + SpiralBurst_ScaleOffset(radius * wave.sample);
        }
        frame->motion.position.y = burst->origin.y;
        frame->motion.position.z =
            burst->origin.z +
            SpiralBurst_ScaleOffset(frame->motion.radius * gUnk_086149C4[frame->motion.angle]);
        sprite->priority = 2;
        frame->depthOffset = -20;
        {
            frame->motion.animationFrame = ParticleRandom_ReadDirectSample(table) & 7;
            if (frame->motion.animationFrame <= 3)
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 12);
            else
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 13);
        }
        frame->motion.lifetime = (ParticleRandom_ReadDirectSample(table) & 15) + 0x38;
        frame = (void *)((u8 *)frame + sizeof(struct SpiralParticle));
        sprite = (void *)((u8 *)sprite + sizeof(struct SpiralParticle));
    }
}
