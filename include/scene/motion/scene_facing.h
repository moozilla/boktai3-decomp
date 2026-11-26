#ifndef GUARD_SCENE_FACING_H
#define GUARD_SCENE_FACING_H

#include "global.h"

static inline u32 SceneFacing_EightWay(u16 angle)
{
    s32 direction = (angle + 112) >> 5;
    u32 mask = 7;
    mask &= direction;
    return mask;
}

static inline u32 SceneFacing_FourWay(u16 angle)
{
    s32 direction = (angle + 96) >> 6;
    u32 mask = 3;
    mask &= direction;
    return mask;
}

#endif
