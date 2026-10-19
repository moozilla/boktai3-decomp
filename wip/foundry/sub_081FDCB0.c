#include "global.h"
#define OrbitEffect_UpdateHomingTrailSprite sub_081FDCB0
#define gOrbitEffectSineTable               gUnk_086149C4
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
static inline void MakeVelocity(struct OrbitEffectPosition *out, s32 angle, s32 speed)
{
    s32 v = gUnk_086149C4[(angle + 64) & 255] * speed;
    s32 x;
    u32 zero;
    if (v >= 0)
        x = v >> 12;
    else
        x = -((-v) >> 12);
    zero = 0;
    out->x = x;
    out->y = zero;
    v = gUnk_086149C4[angle & 255] * speed;
    if (v >= 0)
        v >>= 12;
    else
        v = -((-v) >> 12);
    out->z = v;
}
void sub_081FDCB0(struct OrbitTrailSprite *p)
{
    struct OrbitEffectPosition velocity;
    struct OrbitEffectPosition *velocityPtr;
    u32 age = p->age + 1;
    velocityPtr = 0;
    p->age = age;
    if (p->age <= 31) {
        s32 x = p->position.x - p->target->x;
        struct OrbitEffectPosition *target = p->target;
        s32 z;
        if (x < 0)
            x = -x;
        z = p->position.z - target->z;
        if (z < 0)
            z = -z;
        if (x > 63 || z > 63) {
            if (!(p->age & 3)) {
                s32 desired = (s8)Math_Atan2(target->x - p->position.x, target->z - p->position.z);
                s32 delta = (s8)(desired - (u16)p->angle);
                if (delta > 20)
                    p->angle += 20;
                else if (delta < 20)
                    p->angle -= 20;
                else
                    p->angle = desired;
                velocityPtr = &velocity;
                MakeVelocity(velocityPtr, p->angle, p->radius);
                p->velocityX = velocity.x;
                p->velocityZ = velocity.z;
            }
            p->position.x += p->velocityX;
            p->position.z += p->velocityZ;
            return;
        }
    }
    p->active = (u32)velocityPtr;
    p->flags |= 1;
}
