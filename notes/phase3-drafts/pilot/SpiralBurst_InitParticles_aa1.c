#include "engine/math/monster_particle_random.h"
#include "effects/bursts/spiral_burst.h"

#include "engine/engine_globals.h"
#include "engine/ram.h"

static inline u32 SpiralBurst_Random(const u16 *table, u32 mask)
{
    u32 sample;
    gRandomIndex = (gRandomIndex + 1) & mask;
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, gRandomIndex);
    return sample;
}

void SpiralBurst_InitParticles(struct SpiralBurst *burst)
{
    const u16 *table;
    u32 mask;
    s32 i;
    u8 phase;
    void *sprite;
    struct SpiralParticleFrame *frame;
    {
        const u16 *top = gRandomTable;
        u32 next = gRandomIndex + 1;
        mask = 1023;
        gRandomIndex = next & mask;
        phase = *(const u8 *)(gRandomIndex * 2 + (u32)top);
    }
    sprite = burst->particles;
    burst->resource = AnimData_FindEntry(0x1C1E);
    i = 0;
    table = gRandomTable;
    frame = &burst->particles[0].frame;
    for (; i < 8; i++) {
        ParticleSprite_Initialize(sprite, burst->resource, 0);
        Effect_SetOrigin(sprite, -4, -4);
        Effect_SetPalette(sprite, 1);
        frame->motion.angle = phase + (i << 5) + (SpiralBurst_Random(table, mask) & 15);
        frame->motion.riseRate = ((i & 3) << 1) + 3;
        frame->motion.radius = (SpiralBurst_Random(table, mask) & 31) + 64;
        frame->motion.riseDelay = (SpiralBurst_Random(table, mask) & 15) + 12;
        frame->motion.angularSpeed = 7;
        {
            union {
                const s16 *table;
                s16 sample;
            } wave;
            s32 radius = frame->motion.radius;
            wave.table = gUnk_086149C4;
            wave.sample = wave.table[((frame->motion.angle + 64) & 255)];
            frame->motion.position.world.x =
                burst->origin.x + SpiralBurst_ScaleOffset(radius * wave.sample);
        }
        frame->motion.position.world.y = burst->origin.y;
        frame->motion.position.world.z =
            burst->origin.z +
            SpiralBurst_ScaleOffset(frame->motion.radius * gUnk_086149C4[frame->motion.angle]);
        ((struct SpiralParticle *)sprite)->renderMode = 2;
        frame->status = 0xEC;
        {
            u32 v = SpiralBurst_Random(table, mask) & 7;
            frame->motion.animationFrame = v;
            if (v <= 3)
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 12);
            else
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 13);
        }
        frame->motion.lifetime = (SpiralBurst_Random(table, mask) & 15) + 0x38;
        frame = (void *)((u8 *)frame + sizeof(struct SpiralParticle));
        sprite = (u8 *)sprite + sizeof(struct SpiralParticle);
    }
}
