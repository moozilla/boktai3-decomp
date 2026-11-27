#include "global.h"
struct SceneMoverPosition {
    s16 x, y, z;
};
struct SceneMover {
    u8 unknown0[0x110];
    struct SceneMoverPosition *anchor;
    u8 unknown114[0x217];
    u8 side;
    u8 unknown32C[0x70];
    struct SceneMoverPosition target;
};
static inline void SceneMover_SetVector(struct SceneMoverPosition *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void SceneMover_SetSideTarget(struct SceneMover *p)
{
    struct SceneMoverPosition *anchor = p->anchor;
    struct SceneMoverPosition *other = anchor;
    if (p->side == 0) {
        SceneMover_SetVector(&p->target, 1024, 832, anchor->z);
    } else {
        SceneMover_SetVector(&p->target, other->x, 832, 1024);
    }
}
