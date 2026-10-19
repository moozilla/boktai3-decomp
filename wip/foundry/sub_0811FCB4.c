#include "global.h"
#define ActorMotion_SetKnockback sub_0811FCB4
#define gSineTable               gUnk_086149C4
struct KnockbackBounds {
    u8 unknown0[0x10];
    s16 position[3];
    u8 unknown16[0xA];
    s16 offset[3];
};
struct KnockbackActor {
    u8 unknown0[0x56C];
    struct KnockbackBounds bounds;
    u8 unknown592[0x36];
    s32 velocityX;
    u32 unknown5D0;
    s32 velocityZ;
    u8 unknown5D8[0x14];
    u8 timer;
};
typedef struct {
    s32 x, y, z, pad;
} KnockbackVector;
typedef struct {
    s16 x, y, z, pad;
} KnockbackStep;
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline void BoundsCenter(KnockbackVector *v, struct KnockbackBounds *b)
{
    v->x = b->position[0] + b->offset[0];
    v->y = b->position[1] + b->offset[1];
    v->z = b->position[2] + b->offset[2];
}
static inline void Subtract(KnockbackVector *v, KnockbackVector *a, KnockbackVector *b)
{
    v->x = a->x - b->x;
    v->y = a->x - b->x;
    v->z = a->x - b->x;
}
static inline s32 ScaleStep(s32 a)
{
    if (a >= 0)
        return a >> 12;
    return -((-a) >> 12);
}
static inline void Direction(KnockbackStep *v, s32 angle, s32 speed)
{
    v->x = ScaleStep(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = ScaleStep(gUnk_086149C4[angle & 255] * speed);
}
void sub_0811FCB4(struct KnockbackActor *actor, struct KnockbackBounds *other)
{
    KnockbackVector b, a, delta;
    KnockbackStep step;
    s32 angle;
    BoundsCenter(&a, &actor->bounds);
    BoundsCenter(&b, other);
    Subtract(&delta, &a, &b);
    angle = Math_Atan2(delta.x, delta.z);
    Direction(&step, angle, 10);
    actor->velocityX = step.x << 12;
    actor->velocityZ = step.z << 12;
    actor->timer = 30;
}
