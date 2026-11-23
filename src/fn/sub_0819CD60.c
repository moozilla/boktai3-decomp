#include "global.h"
#include "hresvelgr_travel.h"
extern const s16 gUnk_086149C4[];
u16 Sqrt(u32);
s32 Div(s32, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void Normalize(struct SceneVec *v)
{
    struct SceneVec *out = v;
    s32 speed = 12;
    u16 length = Sqrt(v->x * v->x + out->y * out->y + out->z * out->z);
    if (length) {
        v->x = Div(v->x * speed, length);
        out->y = Div(out->y * speed, length);
        out->z = Div(out->z * speed, length);
    }
}
static inline s32 Dot(struct SceneVec *a, struct SceneVec *b)
{
    return a->x * b->x + a->z * b->z;
}
void Hresvelgr_SteerTowardTarget(struct HresvelgrTravelActor *input)
{
    struct HresvelgrTravelActor *a = input;
    struct SceneVec delta, left, right;
    register struct SceneVec *v = &a->delta;
    register s32 y;
    struct SceneVec *out = &delta;
    struct SceneVec *target = &a->target, *position = &a->position;
    delta.x = target->x - position->x;
    out->y = target->y - position->y;
    out->z = target->z - position->z;
    y = out->y;
    if ((u32)(y + 15) <= 30) {
        s32 value = -16;
        if (y > 0)
            value = 16;
        y = value;
    }
    y = y >= 0 ? y >> 4 : -(-y >> 4);
    Normalize(&delta);
    {
        s32 x = Scale(v->x * gUnk_086149C4[68] - v->z * gUnk_086149C4[4]);
        s32 yy = v->y;
        s32 z = Scale(v->x * gUnk_086149C4[4] + v->z * gUnk_086149C4[68]);
        struct SceneVec *q = &left;
        q->x = x;
        q->y = yy;
        q->z = z;
    }
    {
        s32 x = Scale(v->x * gUnk_086149C4[68] + v->z * gUnk_086149C4[4]);
        s32 yy = v->y;
        s32 z = Scale(-v->x * gUnk_086149C4[4] + v->z * gUnk_086149C4[68]);
        struct SceneVec *q = &right;
        q->x = x;
        q->y = yy;
        q->z = z;
    }
    if (Dot(v, &left) <= Dot(v, &delta)) {
        v->x = delta.x;
        v->z = delta.z;
    } else if (Dot(&left, &delta) <= Dot(&right, &delta))
        *v = right;
    else
        *v = left;
    Normalize(v);
    v->y = y;
    a->velocity = *v;
}
