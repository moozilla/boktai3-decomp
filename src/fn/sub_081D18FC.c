#include "global.h"

void TaggedSceneNpc_UpdateBehavior(u32, u32);

s32 TaggedSceneNpc_OnBehaviorUpdate(u32 a, u32 b)
{
    TaggedSceneNpc_UpdateBehavior(a, b);
    return 0;
}
