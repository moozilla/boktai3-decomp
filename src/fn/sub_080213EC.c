#include "global.h"

extern u32 gUnk_020000B8[];
void SceneActors_Register(u32 v, u32 i)
{
    gUnk_020000B8[i] = v;
}
