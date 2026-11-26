#ifndef GUARD_SCENE_VEC_POLAR_H
#define GUARD_SCENE_VEC_POLAR_H

#include "engine/shared/scene_vec.h"

extern const s16 gUnk_086149C4[];

static inline s32 SceneVec_TruncateComponent(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}

static inline void SceneVec_SetPolarXZ(struct SceneVec *out, s32 angle, s32 radius)
{
    out->x = SceneVec_TruncateComponent(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = SceneVec_TruncateComponent(gUnk_086149C4[angle & 255] * radius);
}

#endif
