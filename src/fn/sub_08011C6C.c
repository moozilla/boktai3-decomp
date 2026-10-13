#include "scene_indicator.h"

struct SceneIndicator *sub_08011C6C(struct SceneIndicators *p, u32 key)
{
    struct SceneIndicator *q = p->indicators;
    s32 i = 0;
    while (i <= 15) {
        if (q->active != 0 && q->id == key) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}
