#include "hresvelgr_travel.h"
#include "scene_facing.h"

struct HresvelgrAngles {
    s32 a[2];
};
extern const struct HresvelgrAngles gUnk_0824F724;
extern const s16 gUnk_086149C4[];
u16 Sqrt(u32);
static inline s32 Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
static inline void Polar(struct SceneVec *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 0x40) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Add(struct SceneVec *a, struct SceneVec *b)
{
    a->x = b->x + a->x;
    a->y = a->y + b->y;
    a->z = b->z + a->z;
}
static inline void Subtract(struct SceneVec *a, struct SceneVec *b)
{
    a->x = a->x - b->x;
    a->y = a->y - b->y;
    a->z = a->z - b->z;
}
s32 Hresvelgr_RefreshTarget(struct HresvelgrTravelActor *p)
{
    struct HresvelgrAngles angles = gUnk_0824F724;
    struct SceneVec v;
    struct HresvelgrTravelActor *obj = p;
    Polar(&v, (p->side << 6) | 0x80, 0x300);
    {
        struct SceneVec *o = obj->origin;
        struct SceneVec *vp;
        vp = &v;
        Add(vp, o);
    }
    p->target = v;
    obj->facing = angles.a[p->side];
    obj->facing4 = SceneFacing_FourWay(obj->facing);
    obj->facing8 = SceneFacing_EightWay(obj->facing);
    Subtract(&v, &obj->position);
    p->travel = Sqrt(v.x * v.x + v.z * v.z) * 0x274 / 0x6400;
    /* The four travel-state calls discard the return register. */
}
