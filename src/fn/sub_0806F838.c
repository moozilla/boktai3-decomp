#include "scene_range_watch.h"
s32 Mod(s32, s32);
u32 SceneRangeWatch_Tick(struct SceneRangeWatch *p)
{
    if (p->interval == 0)
        SceneRangeWatch_CheckRange(p);
    else if (Mod(p->frame, p->interval) == 0)
        SceneRangeWatch_CheckRange(p);
    p->frame += 1;
    return 0;
}
