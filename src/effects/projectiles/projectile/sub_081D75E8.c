#include "effects/projectiles/timed_effect_manager.h"


extern const s16 gUnk_086149C4[];

struct TrailPosition {
    u16 x, y, z, unk_06;
};
/* A position-first view of an entry, retaining the sprite array stride. */
struct TrailMotion {
    struct TrailPosition position;
    u8 unk_08[8];
    u8 active;
    s8 angleOffset;
    u16 radius;
    u16 timer;
    u8 unk_16[0x1A];
};
struct PositionWords {
    u32 a, b;
};
static inline s32 Truncate(s32 product)
{
    if (product >= 0)
        return product >> 12;
    return -((-product) >> 12);
}

// Tentative: shrink the four radial trail sprites, hiding each after its fifteenth tick.
void ProjectileTask_UpdateTrailSprites(struct TimedEffectManager *manager, struct Task4E01Projectile *source)
{
    s32 direction;
    u8 facing;
    s32 remaining;
    s32 byteOffset;
    struct TrailMotion *motion;
    s32 angleMask;
    u16 *age;
    direction = (facing = ((u16)source->params.direction + 0x70) >> 5) & 7;
    direction = ((direction + 5) & 7) << 5;
    angleMask = 255;
    byteOffset = 0;
    motion = (struct TrailMotion *)&manager->effects[0].sprite.position;
    age = &motion->timer;
    remaining = 3;
    do {
        if (motion->active) {
            if (++motion->timer > 14) {
                struct TimedEffectTrail *sprite;
                motion->active = 0;
                sprite = manager->effects;
                sprite = (struct TimedEffectTrail *)((u8 *)sprite + byteOffset);
                sprite->sprite.flags |= 1;
            } else {
                s32 radius;
                s32 angle;
                s32 offset;
                struct PositionWords *out;
                radius = (motion->radius * (16 - *age)) >> 3;
                offset = motion->angleOffset + 256;
                angle = (direction + offset) & angleMask;
                out = (struct PositionWords *)&manager->effects[0].sprite.position;
                out = (struct PositionWords *)((u8 *)out + byteOffset);
                *out = *(struct PositionWords *)&source->sprite.position;
                motion->position.x += Truncate(gUnk_086149C4[(angle + 64) & angleMask] * radius);
                motion->position.z += Truncate(gUnk_086149C4[angle & angleMask] * radius);
            }
        }
        byteOffset += sizeof(struct TimedEffectTrail);
        motion++;
        age += sizeof(struct TimedEffectTrail) / sizeof(u16);
    } while (--remaining >= 0);
}
