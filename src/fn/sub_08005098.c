#include "global.h"

extern const u8 gUnk_086049AC[];
u32 SceneObjects_TryPushNeighbor(u32 a, u32 b);

u32 sub_08005098(u32 a, u32 i)
{
    return SceneObjects_TryPushNeighbor(a, gUnk_086049AC[i]);
}
