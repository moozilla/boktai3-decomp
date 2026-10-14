#include "global.h"
s32 sub_0811AB78(s32, u32 *, u32 *);
void ScenePool_ReadActorBounds(s32 i, u32 *a, u32 *b, u32 *regions)
{
    if (sub_0811AB78(i, a, b) < 0) {
        *a = 0;
        *b = 0;
        regions[0] = 0;
        regions[1] = 0;
    } else {
        regions[0] = ((u8 *)a)[3];
        regions[1] = ((u8 *)b)[3];
    }
}
