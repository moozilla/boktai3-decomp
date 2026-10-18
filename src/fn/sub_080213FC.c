#include "global.h"

extern u32 gUnk_020000B8[];
void SceneActors_Unregister(u32 i)
{
    gUnk_020000B8[i] = 0;
}
