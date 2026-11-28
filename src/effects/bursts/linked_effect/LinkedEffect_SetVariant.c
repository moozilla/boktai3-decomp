#include "effects/bursts/spiral_burst.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"

u32 DebrisMote_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);

void LinkedEffect_SetVariant(struct ContactPosition *position, u8 variant)
{
    s32 i;
    const u16 *table;
    u32 mask;
    struct ContactPosition velocity;
    struct ContactPosition *planar;
    s32 angle;
    u32 *cursor;

    table = gRandomTable;
    planar = &velocity;
    angle = variant + 214;
    cursor = &gRandomIndex;
    mask = 1023;

    for (i = 0; i < 6; i++) {
        u32 sample;
        union {
            u32 mask, index, offset;
            const u16 *source;
        } read;
        union {
            const s16 *sine;
            u32 heightMask;
        } projection;
        s32 speed;
        u32 heading, height, index;
        u32 lifeSample;

        heading = (u8)angle;
        sample = (*cursor + 1) & mask;
        *cursor = sample;
        PARTICLE_RANDOM_READ_SAMPLE(sample, table, sample);
        read.mask = 3;
        speed = (sample & read.mask) + 4;

        projection.sine = gUnk_086149C4;
        planar->x = SpiralBurst_ScaleOffset(projection.sine[(heading + 64) & 255] * speed);
        planar->y = 0;
        planar->z = SpiralBurst_ScaleOffset(projection.sine[heading] * speed);

        index = (*cursor + 1) & mask;
        PARTICLE_RANDOM_READ_SAMPLE(height, table, index);
        projection.heightMask = 15;
        planar->y = height & projection.heightMask;

        read.index = (index + 1) & mask;
        *cursor = read.index;
        read.offset = read.index * sizeof(*table);
        read.source = (const u16 *)((const u8 *)table + read.offset);
        lifeSample = *read.source;
        sample = 15;
        DebrisMote_Create(position, &velocity, (lifeSample & sample) + 30, 5);
        angle += 14;
    }
}
