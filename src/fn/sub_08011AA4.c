#include "scene_indicator.h"

u32 sub_08011AA4(struct SceneIndicators *s)
{
    struct SceneIndicator *e = s->indicators;
    s32 i;
    for (i = 15; i >= 0; i--, e++) {
        sub_08011A18(s, e);
    }
    return (u32)(gUnk_0200004C = 0);
}
