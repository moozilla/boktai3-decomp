// MATCHING_CONCESSION: register-pinning
#include "contact_position.h"
#include "engine.h"
#include "ram.h"

#define LinkedEffect_CaptureRandomTable(dst)                                                       \
    do {                                                                                           \
        (dst) = gRandomTable;                                                                      \
    } while (0)

u32 SceneMovingParticle_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);

static inline s32 ScaleComponent(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void LinkedEffect_SetVariant(struct ContactPosition *position, u8 variant)
{
    s32 count;
    const u16 *table;
    u32 mask;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    s32 angle;
    u32 *cursor;

    LinkedEffect_CaptureRandomTable(table);
    v = &velocity;
    angle = variant + 214;
    cursor = &gRandomIndex;
    mask = 1023;
    count = 5;

    do {
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
        sample = table[sample];
        read.mask = 3;
        speed = (sample & read.mask) + 4;

        projection.sine = gUnk_086149C4;
        v->x = ScaleComponent(projection.sine[(heading + 64) & 255] * speed);
        v->y = 0;
        v->z = ScaleComponent(projection.sine[heading] * speed);

        index = (*cursor + 1) & mask;
        height = table[index];
        projection.heightMask = 15;
        v->y = height & projection.heightMask;

        read.index = (index + 1) & mask;
        *cursor = read.index;
        read.offset = read.index * sizeof(*table);
        read.source = (const u16 *)((const u8 *)table + read.offset);
        lifeSample = *read.source;
        sample = 15;
        SceneMovingParticle_Create(position, &velocity, (lifeSample & sample) + 30, 5);
        angle += 14;
    } while (--count >= 0);
}
