#include "global.h"
#include "objects/sun_orb/sun_orb_effect.h"

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
void OrbitEffect_UpdateHomingTrail(struct OrbitTrailSprite *trail, void *resource)
{
    if ((u16)++trail->age < 32) {
        struct OrbitEffectPosition *target;
        s32 dx, dz;
        dx = trail->position.x - trail->target->x;
        target = trail->target;
        if (dx < 0)
            dx = -dx;
        dz = trail->position.z - target->z;
        if (dz < 0)
            dz = -dz;
        /* Both end paths are written out; jump2 merges them into one tail. */
        if (dx <= 63 && dz <= 63) {
            trail->active = 0;
            trail->flags |= 1;
            return;
        }
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
    trail->active = 0;
    trail->flags |= 1;
}
