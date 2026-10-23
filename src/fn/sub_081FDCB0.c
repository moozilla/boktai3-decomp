// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct OrbitEffectPosition {
    s16 x, y, z, pad;
};
struct OrbitTrailSprite {
    u32 flags;
    u8 unknown4[0x18 - 4];
    struct OrbitEffectPosition position;
    u8 unknown20[8];
    struct OrbitEffectPosition *target;
    u16 active, age;
    s16 angle, radius, velocityX, velocityZ;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Polar(struct OrbitEffectPosition *p, u32 angle, s32 radius)
{
    p->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    p->y = 0;
    p->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
// confidence=low: advances a timed trail toward the owning effect's current position.
#define OrbitEffect_UpdateHomingTrail sub_081FDCB0
void OrbitEffect_UpdateHomingTrail(struct OrbitTrailSprite *trail)
{
    u32 age = trail->age + 1;
    u32 zero = 0;
    asm("" : : "r"(zero));
    trail->age = age;
    if ((u16)age <= 31) {
        s32 dx = trail->position.x;
        register struct OrbitEffectPosition *source asm("r1") = trail->target;
        struct OrbitEffectPosition *target;
        s32 dz;
        dx -= source->x;
        target = source;
        if (dx < 0)
            dx = -dx;
        dz = trail->position.z - target->z;
        if (dz < 0)
            dz = -dz;
        if (dx > 63 || dz > 63) {
            /* Preserve the observed position reloads before steering. */
            asm("" : : : "memory");
            if (!(trail->age & 3)) {
                struct OrbitEffectPosition velocity;
                u32 heading =
                    Math_Atan2(target->x - trail->position.x, target->z - trail->position.z);
                u32 current = (u16)trail->angle;
                s32 desired = (s8)heading;
                s32 delta = (s8)(desired - current);
                if (delta > 20)
                    trail->angle = current + 20;
                else if (delta <= 19)
                    trail->angle = current - 20;
                else
                    trail->angle = desired;
                Polar(&velocity, trail->angle, trail->radius);
                trail->velocityX = velocity.x;
                trail->velocityZ = velocity.z;
            }
            trail->position.x += trail->velocityX;
            trail->position.z += trail->velocityZ;
            return;
        }
    }
    trail->active = zero;
    trail->flags |= 1;
}
