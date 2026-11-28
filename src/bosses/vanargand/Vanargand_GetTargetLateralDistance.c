#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

u16 Sqrt(u32);

static inline s32 Shift(s32 v, s32 n)
{
    s32 r;
    if (v >= 0)
        r = v >> n;
    else
        r = -((-v) >> n);
    return r;
}

static inline void Project(struct SceneVec *out, struct SceneVec *p)
{
    s32 x = p->x;
    s32 z = p->z;
    s32 sum, y;
    out->x = Shift((x - z) * 3, 5);
    sum = Shift((x + z) * 3, 5);
    y = Shift(p->y * 3, 5);
    out->y = sum - y;
    out->z = sum + y;
}

static inline void SetVector(struct SceneVec *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

static inline void Unproject(struct SceneVec *out, struct SceneVec *p, s32 y)
{
    s32 x = p->x;
    s32 h = p->y;
    s32 v;
    v = ((x + h) << 5) / 3 + y;
    out->x = Shift(v, 1);
    out->y = y;
    v = ((h - x) << 5) / 3 + y;
    out->z = Shift(v, 1);
}

// Tentative: measures target separation along the projected horizontal axis.

u32 Vanargand_GetTargetLateralDistance(struct VanargandPairAttack *boss)
{
    struct SceneVec origin, target, difference, world;
    struct SceneVec **destination;

    Project(&origin, &boss->position);
    destination = &boss->destination;
    Project(&target, *destination);
    SetVector(&difference, target.x - origin.x, 0, 0);
    Unproject(&world, &difference, 0);
    return Sqrt(world.x * world.x + world.z * world.z);
}
