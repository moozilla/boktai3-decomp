#define gSineTable gUnk_086149C4
#include "global.h"
#include "scene_vec.h"
extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

void SceneVec_OffsetPolar(struct SceneVec *out, const struct SceneVec *in, s32 angle, s32 distance)
{
    out->x = in->x + scale(gUnk_086149C4[(angle + 64) & 255] * distance);
    out->y = in->y;
    out->z = in->z + scale(gUnk_086149C4[angle & 255] * distance);
}
