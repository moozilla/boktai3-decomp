#ifndef GUARD_SCENE_VEC_H
#define GUARD_SCENE_VEC_H

#include "global.h"

struct SceneVec {
    s16 x, y, z;
    s16 pad;
};

static inline void SceneVec_Copy(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}

#endif
