#include "global.h"

// confidence=low: compares projected horizontal positions within a fixed margin.
#define Vanargand_TestProjectedHorizontalRange sub_081A6A08

struct VanargandProjectionSource {
    u8 pad0[0x54];
    s16 position[3];
    u8 pad5A[0xB6];
    s16 *targetPosition;
};

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

static inline void Project(s16 *out, s16 *p)
{
    s32 x = p[0];
    s32 z = p[2];
    s32 sum, y;
    out[0] = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(p[1]);
    out[1] = sum - y;
    out[2] = sum + y;
}

u32 Vanargand_TestProjectedHorizontalRange(struct VanargandProjectionSource *s)
{
    s16 origin[3], target[3];
    Project(origin, s->position);
    {
        s16 **slot = &s->targetPosition;
        Project(target, *slot);
    }
    if (target[0] > origin[0] - 96 && target[0] < origin[0] + 96)
        return 1;
    return 0;
}
