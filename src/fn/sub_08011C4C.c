#include "scene_indicator.h"

struct SceneIndicator *sub_08011C4C(struct SceneIndicators *p)
{
    struct SceneIndicator *q = p->indicators;
    s32 i = 0;
    while (i <= 15) {
        if (q->active == 0) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}
