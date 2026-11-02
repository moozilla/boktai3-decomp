#ifndef GUARD_SCENE_EFFECT_PROJECTION_H
#define GUARD_SCENE_EFFECT_PROJECTION_H

#include "contact_position.h"
#include "render.h"

static inline s32 SceneEffect_ScaleProjection(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

#define SET_PROJECTED_EFFECT_SIDE(position, side)                                                  \
    do {                                                                                           \
        struct ContactPosition projected;                                                          \
        s32 rightSide;                                                                             \
        struct ContactPosition *output;                                                            \
        const struct ContactPosition *input;                                                       \
        s32 x, z, sum, screenY, height, depth;                                                     \
        output = &projected;                                                                       \
        input = (position);                                                                        \
        x = input->x;                                                                              \
        z = input->z;                                                                              \
        output->x = SceneEffect_ScaleProjection(3 * (x - z));                                                  \
        sum = SceneEffect_ScaleProjection(3 * (x + z));                                                        \
        height = SceneEffect_ScaleProjection(3 * input->y);                                                    \
        screenY = sum - height;                                                                    \
        rightSide = 0;                                                                             \
        depth = sum + height;                                                                      \
        output->x = output->x - gCameraOrigin[0] + 120;                                            \
        output->y = screenY - gCameraOrigin[1] + 80;                                               \
        output->z = depth - gCameraOrigin[2];                                                      \
        if (projected.x > 120)                                                                     \
            (side) = rightSide;                                                                    \
        else                                                                                       \
            (side) = 1;                                                                            \
    } while (0)

#endif
