#include "scene_range_watch.h"
#define SceneRangeWatch_Tick sub_0806F838
s32 Mod(s32, s32);
u32 SceneRangeWatch_Tick(struct SceneRangeWatch *p)
{
    if (p->interval == 0)
        sub_0806F7B0(p);
    else if (Mod(p->frame, p->interval) == 0)
        sub_0806F7B0(p);
    p->frame += 1;
    return 0;
}
