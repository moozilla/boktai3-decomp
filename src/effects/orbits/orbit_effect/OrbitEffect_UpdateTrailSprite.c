#include "global.h"
#define gOrbitEffectSineTable gUnk_086149C4
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
    s16 angle, radius;
};
extern const s16 gUnk_086149C4[];
void OrbitEffect_UpdateTrailSprite(struct OrbitTrailSprite *p)
{
    s32 radius;
    u32 age = p->age + 1;
    radius = 0;
    p->age = age;
    if (p->age <= 15) {
        s32 x = p->position.x - p->target->x;
        struct OrbitEffectPosition *target = p->target;
        s32 z;
        s32 v, delta;
        if (x < 0)
            x = -x;
        z = p->position.z - target->z;
        if (z < 0)
            z = -z;
        if (x > 127 || z > 127) {
            radius = (p->radius * (16 - p->age)) >> 4;
            p->angle = (p->angle + 1) & 255;
            v = gUnk_086149C4[(p->angle + 64) & 255] * radius;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            p->position.x = target->x + delta;
            v = gUnk_086149C4[p->angle & 255] * radius;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            p->position.z = target->z + delta;
            return;
        }
    }
    p->active = radius;
    p->flags |= 1;
}
