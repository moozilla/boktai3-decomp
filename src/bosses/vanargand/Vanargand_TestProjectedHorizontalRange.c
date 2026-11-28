#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

// confidence=low: compares projected horizontal positions within a fixed margin.

static inline s32 Scale(s32 v)
{
    s32 t = v * 3;
    s32 r;
    if (t >= 0)
        r = t >> 5;
    else
        r = -((-t) >> 5);
    return r;
}

static inline void Project(struct SceneVec *out, struct SceneVec *p)
{
    s32 x = p->x;
    s32 z = p->z;
    s32 sum, y;
    out->x = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(p->y);
    out->y = sum - y;
    out->z = sum + y;
}

u32 Vanargand_TestProjectedHorizontalRange(struct VanargandPairAttack *boss)
{
    struct SceneVec origin, target;
    struct SceneVec **destination;

    Project(&origin, &boss->position);
    destination = &boss->destination;
    Project(&target, *destination);
    if (target.x > origin.x - 96 && target.x < origin.x + 96)
        return 1;
    return 0;
}
