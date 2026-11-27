#include "global.h"
struct SceneMoverPosition {
    s16 x, y, z;
};
struct SceneMover {
    u8 unknown0[0x54];
    struct SceneMoverPosition position;
    u8 unknown5C[0x340];
    struct SceneMoverPosition target;
};
static inline void SceneMover_SubtractVector(struct SceneMoverPosition *out,
                                             const struct SceneMoverPosition *a,
                                             const struct SceneMoverPosition *b)
{
    out->x = (u16)a->x - (u16)b->x;
    out->y = (u16)a->y - (u16)b->y;
    out->z = (u16)a->z - (u16)b->z;
}
s32 SceneMover_IsNearTarget(struct SceneMover *p, s32 threshold)
{
    struct SceneMoverPosition difference;
    s32 x, z, xSquare, zSquare, distance;
    SceneMover_SubtractVector(&difference, &p->target, &p->position);
    x = difference.x;
    xSquare = x * x;
    z = difference.z;
    zSquare = z * z;
    {
        s32 copy = zSquare;
        distance = (s32)((u32)xSquare + (u32)copy);
    }
    if (distance <= threshold * 144)
        return 1;
    return 0;
}
