#include "global.h"
#include "scene/collision/contact_actor.h"
#include "engine/math/monster_particle_random.h"
struct SpriteEffectController;
struct SpriteEffectController *SpriteEffectController_Create(const struct ContactPosition *,
                                                             const struct ContactPosition *, s32,
                                                             u32, u32, u32, u32, u32, u32, u32, u32,
                                                             u32, s32, s32, s32, s32, s32, s32);

static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

struct BossScatterEmitter {
    u8 unknown0[0x54];
    struct ContactPosition position;
};

void sub_0817AEC8(struct BossScatterEmitter *p, int direction)
{
    union ParticleRandomTableSource randomSource;
    const u16 *table;
    int i = 0;
    struct ContactPosition offset;
    randomSource.halves[0] = (u16)RANDOM_TABLE_ADDR;
    randomSource.halves[1] = RANDOM_TABLE_ADDR >> 16;
    table = ParticleRandom_GetTable(&randomSource);
    for (; i < 6; i++) {
        struct ContactPosition *v = &offset;
        u32 *cursor = &gRandomIndex;
        int angle = ((s32)ParticleRandom_NextSample(table, cursor, 1023) >> 4) & 63;
        int radius = ((s32)ParticleRandom_NextSample(table, cursor, 1023) >> 4) & 31;
        int life;
        int spread;
        int speed;
        const s16 *sine;
        int heading;
        spread = i * 4;
        heading = angle + direction;
        heading -= 32;
        speed = radius + spread;
        speed += 2;
        sine = (const s16 *)((u32)gUnk_086149C4 & ~((u16)((u8)radius << 8) >> 16));
        if (radius < 0)
            sine++;
        v->x = Scale((sine[((heading + 64) & 255)]) * speed);
        v->y = 0;
        v->z = Scale((sine[(heading & 255)]) * speed);
        life = (((s32)ParticleRandom_NextSample(table, cursor, 1023) >> 4) & 15) + 16;
        SpriteEffectController_Create(&p->position, &offset, 0x1C1E, 0xD43F, 0, 0, 0x11, 1, 1, 2, 0,
                                      life, 0, 0, 0, 0, -4, -4);
    }
}
